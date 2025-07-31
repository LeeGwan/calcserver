
#include <vector>
#include <memory>
struct Struct_ObjectDB;
class Ezreal final
{
public:
	Ezreal();
	~Ezreal();
	static void load(std::vector<std::unique_ptr<Struct_ObjectDB>>& ObjectDB);


};