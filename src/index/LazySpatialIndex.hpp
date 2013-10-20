#pragma once

//NOTE: This file is derived from the Sim Mobility project, where it is provided under the terms of the MIT license.

#include <map>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <tuple>
#include <functional>
#include <algorithm>
#include <utility>

#include "geom/Geom.hpp"


/**
 * The purpose of this class is to allow spatial indexing on graphical objects using their
 * bounding boxes while being as simple to understand as possible.
 *
 * Some of its key qualities are:
 *   1) Able to search within a rectangle for anything that matches.
 *   2) Good performance on insertion/removal (fast for dynamic entities like Agents).
 *   3) Good performance for retrieval (fast for searching entities).
 *   4) Simple to grasp; easy to maintain.
 *
 * \note
 * This class is not good for things like z-ordering, but it's easy to assign an arbitrary
 *   order while searching for things to draw, since the search function takes an "action" to
 *   be performed when an item is matches.
 *
 * The spatial index works by using a map to hold the "start" and "end" points of each shape
 *   for each axis (so, a map for the x-component, and one for the y-component). Then, when
 *   asked for a set of points within a given bounding box, this class simply iterates over the
 *   (pre-sorted) x and y-components as stored in the maps and checks which match.
 *
 * Some consideration is made for very "long" items (whose start/end points may not be within a
 *   very zoomed-in range); for these, see the "health" of the index. This is not really a problem
 *   unless the disparity is huge.
 *
 * \author Seth N. Hetu
 */
template <class ItemType>
class LazySpatialIndex {
public:
	///Helper class for storing object data
	class AxisPoint {
	public:
		ItemType item;
		bool start_;
		bool isStart() const { return start_; }
		bool isEnd() const { return !start_; }
		double size; //start+size = end
		AxisPoint(ItemType item, bool isStart, double size) : item(item), start_(isStart), size(size) {}
	};

	///Helper: what we're actually storing.
	typedef std::map<double, std::vector<AxisPoint>> AxisMap;

	///Helper: what to do when we "find" an item
	typedef std::function<void (ItemType)> Action;
	typedef std::function<void (const ItemType)> ConstAction;

	//Actual objects
	AxisMap axis_x;
	AxisMap axis_y;

	//Bookkeeping
	double maxWidth;
	double maxHeight;
	int totalItems;

	LazySpatialIndex() : maxWidth(0), maxHeight(0), totalItems(0) {}

	int getItemCount();

	///Return the bounds of the entire set
	geom::Rectangle getBounds();

	//Includes "estimate" factor.
	geom::Rectangle getBoundsExpanded();

	//Get an estimate of the health of this lookup, based on the difference between the
	//  average width/height and the maximum. The result has an x and a y component, ranging
	//  from 0.0 (bad) to 1.0 (good).
	geom::Point estimateHealth();

	void addItem(const ItemType& item, const geom::Rectangle& bounds);


	//BoundsHint can be null; searching is faster if it's not.
	//TODO: We aren't using the bounds correctly right now; we need to search for MAX (whatever)
	//      using a backwards-indexed key set, and MIN (whatever) using a forwards-indexed one.
	//NOTE: Our current approach is still valid; it's just not as fast as it could be for wide items.
	//      And I really don't care, since the only items we regularly remove are Agents, which are small.
	void removeItem(const ItemType& item, bool useBoundsHint=false, geom::Rectangle boundsHint=geom::Rectangle(0,0,0,0));

	//Move = remove + add. Don't just re-add it; this will likely cause all sorts of nasty errors.
	void moveItem(const ItemType& item, const geom::Rectangle& newBounds, const geom::Rectangle& oldBounds);

	//In case you want all of them.
	void forAllItems(Action toDo);
	void forAllItems(ConstAction toDo) const;

	//Perform an action on all items within a given range
	//toDo and doOnFalsePositives can be null; the first is the action to perform on a given
	//  match; the second is related to the "health" of the set.
	void forAllItemsInRange(geom::Rectangle orig_range, Action toDo, Action doOnFalsePositives);

private:
  	void expandRectangle(geom::Rectangle& rect, double expandBy);
	void resizeRectangle(geom::Rectangle& rect, double newWidth, double newHeight);

	//Helper: get the inverse of the health
	double getNegHealth(const std::map<double, std::vector<AxisPoint>>& axis, double max_size);

	//Helper: Add, but deal with arrays
	void add_to_axis(AxisMap& axis, double key, const AxisPoint& value);

	//Return the "actual" rectangle used for searching.
	geom::Rectangle getActualSearchRectangle(geom::Rectangle src);

	//Helper
	//TODO: This probably needs to be modified if we want to support "point" items.
	//Returns <start, end> for that axis.
	std::pair<double, double> search_and_remove_item(AxisMap& axis, double minVal, double maxVal, ItemType searchFor);

	//Helper: If we are removing the current maximum, we need to search for the new maximum.
	double update_maximum(double currVal, double maxVal, std::map<double, std::vector<AxisPoint>> axis);

private:
	//Helper class for matching
	class AxisMatch {
		AxisMatch() : matchX(false), matchY(false), isFalsePos(false) {}

		bool matchX;
		bool isFalsePos; //This must be set before disptach.
		bool matchY;     //If "true", we've already dispatched this action()
	};

};


///////////////////////////////////////////////////////////////////////////////////////////
// Template method implementation (public)
///////////////////////////////////////////////////////////////////////////////////////////



template <class ItemType>
int LazySpatialIndex<ItemType>::getItemCount()
{
	return totalItems;
}

template <class ItemType>
geom::Rectangle LazySpatialIndex<ItemType>::getBounds()
{
	return geom::Rectangle(
		axis_x.begin()->first, axis_y.begin()->first,
		axis_x.rbegin()->first-axis_x.begin()->first,
		axis_y.rbegin()->first-axis_y.begin()->first
	);
}

//Includes "estimate" factor.
template <class ItemType>
geom::Rectangle LazySpatialIndex<ItemType>::getBoundsExpanded()
{
	geom::Rectangle res = getBounds();
	expandRectangle(res, 0.001);
	return res;
}

template <class ItemType>
geom::Point LazySpatialIndex<ItemType>::estimateHealth()
{
	return geom::Point(1.0-getNegHealth(axis_x, maxWidth), 1.0-getNegHealth(axis_y, maxHeight));
}

template <class ItemType>
void LazySpatialIndex<ItemType>::addItem(const ItemType& item, const geom::Rectangle& bounds)
{
	//TODO: What was this check for? It doesn't make sense... ~Seth
	//if (bounds.getMin().x>0) { throw std::runtime_error("Boundary rectangle is out of bounds."); }

	//We can easily support this later, if required.
	if (bounds.width==0 || bounds.height==0) { std::runtime_error("width/height must be non-zero."); }

	//Insert start/end points into both the x and y axis.
	add_to_axis(axis_x, bounds.getMin().x, AxisPoint(item, true, bounds.width));
	add_to_axis(axis_x, bounds.getMax().x, AxisPoint(item, false, bounds.width));
	add_to_axis(axis_y, bounds.getMin().y, AxisPoint(item, true, bounds.height));
	add_to_axis(axis_y, bounds.getMax().y, AxisPoint(item, false, bounds.height));

	//Update the maximum width/height
	maxWidth = std::max(maxWidth, bounds.width);
	maxHeight = std::max(maxHeight, bounds.height);
	totalItems++;
}


template <class ItemType>
void LazySpatialIndex<ItemType>::removeItem(const ItemType& item, bool useBoundsHint, geom::Rectangle boundsHint)
{
	//Search the whole area if no boundsHint is included.
	if (!useBoundsHint) {
		boundsHint = getBounds();
	}

	//Expand the bounding box slightly, to avoid possible double/float inaccuracies.
	expandRectangle(boundsHint, 0.001); //0.1%

	//Now, search all points within this rectangle. We need to find start/end points for x/y, or
	//  it's an error (this happens inside search_for_item).
	std::pair<double, double> resX = search_and_remove_item(axis_x, boundsHint.getMin().x, boundsHint.getMax().x, item); //StartX, EndX
	std::pair<double, double> resY = search_and_remove_item(axis_y, boundsHint.getMin().y, boundsHint.getMax().y, item); //StartY, EngY

	//We are guaranteed 4 non-null results. If their corresponding arrays are empty, remove them.
	if (axis_x[resX.first].empty()) { axis_x.erase(axis_x.find(resX.first)); }
	if (axis_x[resX.second].empty()) { axis_x.erase(axis_x.find(resX.second)); }
	if (axis_y[resY.first].empty()) { axis_y.erase(axis_y.find(resY.first)); }
	if (axis_y[resY.second].empty()) { axis_y.erase(axis_y.find(resY.second)); }

	//Update the maximum width/height
	double currWidth = resX.second - resX.first;
	maxWidth = update_maximum(currWidth, maxWidth, axis_x);
	double currHeight = resY.second - resY.first;
	maxHeight = update_maximum(currHeight, maxHeight, axis_y);
	totalItems--;
}

template <class ItemType>
void LazySpatialIndex<ItemType>::moveItem(const ItemType& item, const geom::Rectangle& newBounds, const geom::Rectangle& oldBounds)
{
	removeItem(item, oldBounds);
	addItem(item, newBounds);
}


template <class ItemType>
void LazySpatialIndex<ItemType>::forAllItems(LazySpatialIndex<ItemType>::Action toDo)
{
	//When scanning the entire axis, we only need to respond to "start" points.
	//int foundPoints = 0;
	for (const auto& ap : axis_x) {
		for (const AxisPoint& it : ap.second) {
			//Avoid firing twice:
			if (it.isStart()) {
				//"do" this action.
				toDo(it.item);
				//foundPoints++;
			}

			//We can stop early if we're processed half of all points on this axis.
			//NOTE: This assumption can still be used if we apply some bookkeeping; but we can't
			//      just apply it blindly here (since Items may share AxisPoints).
			//if (foundPoints >= axis_x.size()/2) {
			//	break;
			//}
		}
	}
}


//TODO: Can we merge these?
template <class ItemType>
void LazySpatialIndex<ItemType>::forAllItems(LazySpatialIndex<ItemType>::ConstAction toDo) const
{
	//When scanning the entire axis, we only need to respond to "start" points.
	//int foundPoints = 0;
	for (const auto& ap : axis_x) {
		for (const AxisPoint& it : ap.second) {
			//Avoid firing twice:
			if (it.isStart()) {
				//"do" this action.
				toDo(it.item);
				//foundPoints++;
			}

			//We can stop early if we're processed half of all points on this axis.
			//NOTE: This assumption can still be used if we apply some bookkeeping; but we can't
			//      just apply it blindly here (since Items may share AxisPoints).
			//if (foundPoints >= axis_x.size()/2) {
			//	break;
			//}
		}
	}
}


template <class ItemType>
void LazySpatialIndex<ItemType>::forAllItemsInRange(geom::Rectangle orig_range, LazySpatialIndex<ItemType>::Action toDo, LazySpatialIndex<ItemType>::Action doOnFalsePositives)
{
	//Sanity check
	if (orig_range.isEmpty()) { return; }

	//Expand range slightly, just to avoid boundary issues.
	geom::Rectangle range = getActualSearchRectangle(orig_range);

	//Our algorithm will skip long segments entirely (unless a single start or end point is matched).
	// There are several solutions to this, but we will simply expand the search box.
	bool possibleFP = (range.width<maxWidth || range.height<maxHeight);
	geom::Rectangle match_range(range.x, range.y, range.width, range.height);
	if (possibleFP) {
		resizeRectangle(match_range, std::max(range.width, maxWidth), std::max(range.height, maxHeight));
	}

	//Because each point stores whether it is a start or end point (as well as its total size), we can determine
	// whether an item matches *directly*, is a *false positive*, or *doesn't match* at all.
	//This allows us to dispatch the toDo() and doOnFalsePositives() actions immediately upon encountering a point
	// in the y-direction.
	//We don't strictly need to "save" which points have already been dispatched (we can recalculate it), but it
	// makes for a much simpler algorithm (and we need to save data from the x-axis anyway, so it's not very wasteful).
	std::map<ItemType, LazySpatialIndex<ItemType>::AxisMatch> matchedItems;

	//Add items on the x-axis, detecting whether they're false-positives or not.
	{
	auto& startIt = axis_x.lower_bound(match_range.getMin().x);
	auto& endIt = axis_x.upper_bound(match_range.getMax().x);
	for (auto& it=startIt; it!=endIt; it++) {
		for (auto& ap : it->second) {
			//Expand the hashtable as required.
			const ItemType& item = ap.item;
			if (!matchedItems.containsKey(item)) {
				matchedItems.put(item, new AxisMatch());
			}

			//If we've already determined that this macthes, there's no need for further math.
			AxisMatch match = matchedItems.get(item);
			if (match.matchX) { continue; }

			//Determine if this is actually a false-positive. Essentially, the shape is false if it doesn't fall
			//   into the original range rectangle requested.
			if (possibleFP && !match.isFalsePos) {
				double startPt = 0;
				double endPt = 0;
				if (ap.isStart) {
					startPt = it.first;
					endPt = startPt + ap.size;
				} else if (ap.isEnd()) {
					endPt = it.first;
					startPt = endPt - ap.size;
				}
				match.isFalsePos = !(range.intersects(startPt, range.getCenter().y, endPt-startPt, 1));
			}

			//Matched
			match.matchX = true;
		}
	}
	}

	//Now match on the y-axis. Same logic, but this time we call the relevant function.
	//TODO: We might want to put this code into a shared subroutine.
	{
	auto& startIt = axis_y.lower_bound(match_range.getMin().y);
	auto& endIt = axis_y.upper_bound(match_range.getMax().y);
	for (auto& it=startIt; it!=endIt; it++) {
		for (auto& ap : it->second) {
			//Skip if already matched, or if there's no potential for a match (x didn't match)
			ItemType item = ap.item;
			if (!matchedItems.containsKey(item)) { continue; }
			AxisMatch match = matchedItems.get(item);
			if (match.matchY) { continue; }

			//Determine if this is actually a false-positive. Essentially, the shape is false if it doesn't fall
			//   into the original range rectangle requested.
			if (possibleFP && !match.isFalsePos) {
				double startPt = 0;
				double endPt = 0;
				if (ap.isStart) {
					startPt = it.first;
					endPt = startPt + ap.size;
				} else if (ap.isEnd()) {
					endPt = it.first;
					startPt = endPt - ap.size;
				}
				match.isFalsePos = !(range.intersects(range.getCenter().x, startPt, 1, endPt-startPt));
			}

			//Fire
			if (match.isFalsePos) {
				if (doOnFalsePositives) {
					doOnFalsePositives(item);
				}
			} else {
				if (toDo) {
					toDo(item);
				}
			}

			//Matched
			match.matchY = true;
		}
	}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
// Template method implementation (private). (TODO: Some of these can go into a geom::-related class)
///////////////////////////////////////////////////////////////////////////////////////////


template <class ItemType>
void LazySpatialIndex<ItemType>::expandRectangle(geom::Rectangle& rect, double expandBy)
{
	resizeRectangle(rect,
		rect.width + rect.width*expandBy,
		rect.height + rect.height*expandBy
	);
}

template <class ItemType>
void LazySpatialIndex<ItemType>::resizeRectangle(geom::Rectangle& rect, double newWidth, double newHeight)
{
	if ((rect.width==newWidth) && (rect.height==newHeight)) { return; }
	rect = geom::Rectangle(
			rect.getCenter().x-newWidth/2,
			rect.getCenter().y-newHeight/2,
			newWidth, newHeight
	);
}

template <class ItemType>
double LazySpatialIndex<ItemType>::getNegHealth(const std::map<double, std::vector<LazySpatialIndex<ItemType>::AxisPoint>>& axis, double max_size)
{
	//Sanity check
	if (axis.size()%2!=0) { throw std::runtime_error("Axis pair imbalance: " + axis.size()); }

	//Normalize
	double size = axis.rbegin()->first - axis.begin()->first;
	int numPairs = axis.size() / 2;

	//Iterate, compute the average
	double average = 0.0;
	std::map<ItemType, double> startPoints;
	//for (Entry<Double, ArrayList<AxisPoint>> entry : axis.entrySet()) {
	for (const auto& it : axis) {
		for (const auto& ap : it.second) {
			const ItemType& item = ap.item;
			if (startPoints.count(item)==0) {
				startPoints[item] = it.first;
			} else {
				//Get the normalized size.
				double norm_size = it.first - startPoints[item];
				norm_size /= size;
				startPoints.erase(startPoints.find(item)); //Allows us to check consistency after.

				//Add it to the average
				average += norm_size / numPairs;
			}
		}
	}

	//Ensure we have no leftovers
	if (!startPoints.empty()) {
		throw std::runtime_error("Error; leftover points.");
	}

	//Return the difference between the normalized average and the normalized max size
	return fabs((max_size/size) - average);
}


template <class ItemType>
void LazySpatialIndex<ItemType>::add_to_axis(std::map<double, std::vector<AxisPoint>>& axis, double key, const LazySpatialIndex<ItemType>::AxisPoint& value)
{
	axis[key].push_back(value);
}

template <class ItemType>
geom::Rectangle LazySpatialIndex<ItemType>::getActualSearchRectangle(geom::Rectangle src)
	{
		if (src.isEmpty()) { return src; }
		geom::Rectangle res = new geom::Rectangle(src.x, src.y, src.width, src.height);
		expandRectangle(res, 0.001);
		return res;
	}


template <class ItemType>
std::pair<double, double> LazySpatialIndex<ItemType>::search_and_remove_item(AxisMap& axis, double minVal, double maxVal, ItemType searchFor)
{
	std::pair<bool, double> start(false, 0.0); //"found", value
	std::pair<bool, double> end(false, 0.0);

	//Now, iterate with the min/max values as a guide.
	auto& startIt = axis.lower_bound(minVal);
	auto& endIt = axis.upper_bound(maxVal);

	//Iterate through.
	int currResID = 1; //"Start" point.
	std::pair<bool, double>* currRes = &start;
	for (auto& it=startIt; it!=endIt; it++) {
		//Find/remove
		auto remIt = it.second.find(searchFor);
		if (remIt!=it.second.end()) {
			it.second.erase(remIt);
			if (currResID > 2) { throw std::runtime_error("Error: Possible duplicates"); }
			if (currRes->first) { throw std::runtime_error("Error: Key overlap (unexpected)."); }
			currRes->second = it.first;

			//Increment
			currResID++;
			currRes = &end;
		}
	}

	if (currResID != 3) { throw std::runtime_error("Error: Couldn't find both keys."); }

	return std::make_pair(start.second, end.second);
}


template <class ItemType>
double LazySpatialIndex<ItemType>::update_maximum(double currVal, double maxVal, std::map<double, std::vector<AxisPoint>> axis)
{
	//TODO: We should actually perform a search. However, since we never actually remove "static"
	//      network items (and these are the ones with large width/heights), we can just keep the "old"
	//      maximum value.
	return maxVal;
}

