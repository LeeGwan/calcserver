#include "Struct_ObjectDB.h"
#include <vector>
#include <memory>

namespace ObjectDB
{
	inline std::vector<std::unique_ptr<Struct_ObjectDB>> ObjectDBList;
	void initialize();
	void Release();
	bool Get_ObjectDB(Struct_ObjectDB& out, const std::string& obj_name);
}
