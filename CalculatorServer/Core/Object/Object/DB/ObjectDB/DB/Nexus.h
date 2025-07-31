#pragma once

#include <vector>
#include <memory>
struct Struct_ObjectDB;
class Nexus final
{
public:
	Nexus();
	~Nexus();
	static void load(std::vector<std::unique_ptr<Struct_ObjectDB>>& ObjectDB);


};