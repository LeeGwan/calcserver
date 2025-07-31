#pragma once

#include <vector>
#include <memory>
struct Struct_ObjectDB;
class Practice_Tool final
{
public:
	Practice_Tool();
	~Practice_Tool();
	static void load(std::vector<std::unique_ptr<Struct_ObjectDB>>& ObjectDB);


};