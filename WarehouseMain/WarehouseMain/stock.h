#pragma once
#ifndef STOCK_H
#define STOCK_H

#include <map>
#include <vector>
#include <array>
#include "..\..\Warehouse\WarehouseLayout\WarehouseLayout\warehouse_common.h"
#include "warehouse_local.h"



void initializeItemID(std::map<std::string, int> &ItemMap) {
	
	ItemMap.insert(std::pair<std::string, int>("UFO 02 Detector", 1));
	ItemMap.insert(std::pair<std::string, int>("Relaxman Relaxation Capsule", 2));
	ItemMap.insert(std::pair<std::string, int>("Milk Carton Hat", 3));
	ItemMap.insert(std::pair<std::string, int>("Inflatable Toast", 4));
	ItemMap.insert(std::pair<std::string, int>("Five-Pound Fat Replica", 5));
	ItemMap.insert(std::pair<std::string, int>("Proporta Elephant Camouflage Kit", 6));
	ItemMap.insert(std::pair<std::string, int>("Laparoscopic Gastric Bypass", 7));
	ItemMap.insert(std::pair<std::string, int>("Wheelmate Laptop Steering Wheel Desk", 8));
	ItemMap.insert(std::pair<std::string, int>("Hutzler 571 Banana Slicer", 9));
	ItemMap.insert(std::pair<std::string, int>("Accoutrements Horse Head Mask", 10));
	ItemMap.insert(std::pair<std::string, int>("Wolf Urine Lure 32 oz", 11));
	ItemMap.insert(std::pair<std::string, int>("Uranium Ore", 12));
	ItemMap.insert(std::pair<std::string, int>("Tuscan Whole Milk, 1 Gallon", 13));
	ItemMap.insert(std::pair<std::string, int>("Three Wolf Moon Short Sleeve Tee", 14));
	ItemMap.insert(std::pair<std::string, int>("Steering Wheel Attachable Work Surface Tray", 15));
	ItemMap.insert(std::pair<std::string, int>("English Grammar For Dummies", 16));
	ItemMap.insert(std::pair<std::string, int>("Asfour Crystal Tut Anhk Amon Chair", 17));
	ItemMap.insert(std::pair<std::string, int>("English Grammar For Dummies", 18));
	ItemMap.insert(std::pair<std::string, int>("ThinkGeek canned Unicorn Meat", 19));
	ItemMap.insert(std::pair<std::string, int>("Accoutrements Yodelling Pickle", 20));
	ItemMap.insert(std::pair<std::string, int>("Poo-Pourri Before-You-Go Toilet Spray", 21));
	ItemMap.insert(std::pair<std::string, int>("Delicious PhD Darling Costume", 22));
	ItemMap.insert(std::pair<std::string, int>("GEP Grouting, Cleaning and Washing Sponge", 23));
	ItemMap.insert(std::pair<std::string, int>("The Kitty Pass Interior Pet Door", 24));
	ItemMap.insert(std::pair<std::string, int>("Portable Pizza Pouch", 25));
	ItemMap.insert(std::pair<std::string, int>("Creative Arts by Charles Leonard Wiggle Eyes", 26));
	ItemMap.insert(std::pair<std::string, int>("Glow In The Dark Toilet Roll", 27));
	ItemMap.insert(std::pair<std::string, int>("Spandex Bodysuits", 28));
	ItemMap.insert(std::pair<std::string, int>("Bacon Bandages", 29));
	ItemMap.insert(std::pair<std::string, int>("Dab Fidget Spinner T-Shirt Cool Dabbing Tee", 30));
}

std::map<int, ItemInfo> StockInfo_map;					// MAP ID TO info

float GenearteItemMass(int itemid) {

	return (float)itemid / 6.0;
}

// Preallocattion MAP OF ITEMS_ID -> ITEMINFO , EACH WITH NO QUANTITY
void GenerateStockInfo(std::map<int, ItemInfo> &StockInfo)
{
	for (int i = 1; i <= MAXITEMTYPES; ++i) {
		ItemInfo iteminfo_blank(0, GenearteItemMass(i));
		StockInfo.insert(std::make_pair( i, iteminfo_blank));
	}
}

// REQUEST BY MAP, PROBABILY IDEAL FOR GUI
void RandomStockGenerator(std::map<int, int> &Restock, int numtypes) {

	for (int i = 0; i < numtypes; ++i) {
		Restock.insert(std::make_pair(randomnum(1, MAXITEMTYPES) , 1));			// generates map of ids, all of quantity 1
	}
}

// REQUEST BY ARRAY, LIKELY WONT USE
void RandomStockGenerator(std::array<int, MAXWAREHOUSESTOCK> &Restock, int quantity) {

	for (int i = 0; i < quantity; ++i) {
		Restock.at(i) = randomnum(1, MAXITEMTYPES);				// all of quantty 1	
			
	}
}

// UPDATE STOCKINFO MAP BY ARRAY
void StockUpdate(std::map<int, ItemInfo> &StockInfo, std::array<int, MAXWAREHOUSESTOCK> items) {
	//int i = 0;
	//do {
	//	StockInfo.


	//} while (items.at(i) != 0);

}

// UPDATE STOCKINFO MAP BY 1
void StockUpdate(std::map<int, ItemInfo> &StockInfo, int item) {
	
	auto it = StockInfo.find(item);
	if (it != StockInfo.end()) {
		//found 
		it->second.quantity++;
	}

	

}

void StockUpdate(std::map<int, ItemInfo> &StockInfo, int item, WarehouseLocation wl) {

	auto it = StockInfo.find(item);
	if (it != StockInfo.end()) {
		//found 
		it->second.quantity++;
		it->second.locations.push_back(std::make_pair(false, wl));
	}

}

void StockQuery() {

}

#endif //STOCK_H