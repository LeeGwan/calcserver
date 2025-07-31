#pragma once

#include <vector>
#include <memory>
struct Struct_ObjectDB;
class Turret final
{
public:
	Turret();
	~Turret();
	static void load(std::vector<std::unique_ptr<Struct_ObjectDB>>& ObjectDB);


};