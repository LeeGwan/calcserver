#pragma once
#include <vector>
#include <memory>

struct Struct_SpellDB;
class ezreal_spell final
{
private:
	static void load_q_data(std::vector<std::unique_ptr<Struct_SpellDB>>& SpellDB);
	static void load_w_data(std::vector<std::unique_ptr<Struct_SpellDB>>& SpellDB);
	static void load_e_data(std::vector<std::unique_ptr<Struct_SpellDB>>& SpellDB);
	static void load_r_data(std::vector<std::unique_ptr<Struct_SpellDB>>& SpellDB);
public:
	ezreal_spell();
	~ezreal_spell();
	static void load(std::vector<std::unique_ptr<Struct_SpellDB>>& SpellDB);
};