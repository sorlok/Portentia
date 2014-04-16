#include "WalkableMapSlice.hpp"

#include <set>
#include <algorithm>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

extern "C" {
	#include "lauxlib.h"
}
#include <luabind/luabind.hpp>

#include <jsoncpp/json/json.h>

#include "core/GameEngine.hpp"
#include "slices/ConsoleSlice.hpp"
#include "widgets/AbstractGameObject.hpp"
#include "widgets/CircleGameObject.hpp"
#include "widgets/RectangleGameObject.hpp"


WalkableMapSlice::WalkableMapSlice() : Slice(), window(nullptr), geControl(nullptr),
	console(new ConsoleSlice("TODO: lua console.")), bkgrdColor(0xC0, 0xC0, 0x00)
{
}

void WalkableMapSlice::load(const std::string& file)
{
	//Get the path.
	std::string path = "./";
	size_t sl = file.rfind('/');
	if (sl!=std::string::npos) {
		path = file.substr(0, sl+1);
	}

	//Read the root node.
	Json::Value root;
	{
	Json::Reader read;
	std::ifstream inFile(file);
	if (!inFile.is_open()) {
		throw std::runtime_error("Can't load file; doesn't exist.");
	}
	read.parse(inFile, root);
	}

	//Background color.
	bkgrdColor = sf::Color(0xC0, 0xC0, 0x00);
	if (root.isMember("background")) {
		std::string bkgrd = root["background"].asString();
		if (bkgrd.find("0x")==0) {
			bkgrd = bkgrd.substr(2);
		}
		unsigned int res;
		std::stringstream ss;
		ss <<std::hex <<bkgrd;
		ss >> res;
		bkgrdColor = sf::Color(sf::Uint8((res>>16)&0xFF), sf::Uint8((res>>8)&0xFF), sf::Uint8(res&0xFF));
	}

	//Tiles.
	for (std::map<std::string, sf::Texture*>::const_iterator it=tiles.begin(); it!=tiles.end(); it++) {
		delete it->second;
	}
	tiles.clear();
	if (root.isMember("tiles")) {
		const Json::Value& ts = root["tiles"];
		const Json::Value::Members& keys = ts.getMemberNames();
		for (const auto& key : keys) {
			sf::Texture* txt = new sf::Texture();
			if (!txt->loadFromFile(path+ts[key].asString())) {
				std::cout <<"Warn: couldn't load texture: " <<ts[key].asString() <<"\n";
			}
			tiles[key] = txt;
		}
	}

	//Tile map.
	tmap.clear();
	if (root.isMember("tmap") && root["tmap"].isArray()) {
		const Json::Value& ts = root["tmap"];
		for (unsigned int i=0; i<ts.size(); i++) {
			const Json::Value& item = ts[i];
			if (item.isMember("tile") && item.isMember("x") && item.isMember("y")) {
				sf::Sprite res(*tiles[item["tile"].asString()]);
				res.setPosition(item["x"].asInt(), item["y"].asInt());
				tmap.push_back(res);
			}
		}
	}

	//OnUpdate event.
	onupdate = "";
	if (root.isMember("onupdate")) {
		onupdate = root["onupdate"].asString();
	}
}

void WalkableMapSlice::save(const std::string& file)
{
	//TODO: populate, save.
	Json::Value root;
}




YieldAction WalkableMapSlice::activated(GameEngineControl& geControl, Slice* prevSlice, sf::RenderWindow& window)
{
	//Save
	this->window = &window;
	this->geControl = &geControl;

	//Done.
	return YieldAction();
}


void WalkableMapSlice::update(const sf::Time& elapsed, const std::vector<sf::Event::KeyEvent>& typed)
{
	//Sprite movement.
	std::pair<int,int> walk = std::make_pair(0,0);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { walk.second--; }
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { walk.second++; }
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { walk.first--; }
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { walk.first++; }

	//TODO: Actually walk.

	//TODO: Process onupdate for all Sprites.

	//Process onupdate for this map.
	if (!onupdate.empty()) {
		luabind::globals(geControl->lua())["this"] = this;
		if (luaL_dostring(geControl->lua(), onupdate.c_str())!=0) {
			std::cout <<"Error running onupdate lua code: ##{" <<onupdate <<"}##\n";
			std::cout <<"Error is: \"" <<lua_tostring(geControl->lua(), -1) <<"\"\n";
		}
	}
}


void WalkableMapSlice::render()
{
	//Nothing to draw.
	if (!window) { return; }

	//Color the background.
	window->clear(bkgrdColor);

	//Color all entries in the tile map.
	for (auto& item : tmap) {
		window->draw(item);
	}
}

void WalkableMapSlice::changeBgColor(long elapsedMs)
{
	//Just make it redder.
	bkgrdColor = sf::Color(bkgrdColor.r+1, bkgrdColor.g, bkgrdColor.b);
}

