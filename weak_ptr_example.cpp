// build: g++ -Wall weak_ptr_example.cpp -o weak_ptr_example.exe
#include <stdio.h>
#include <memory>
#include <string>
#include <vector>

struct Parent;

struct Child {
   std::string name;
   std::weak_ptr<Parent> parent;                    // Child only observes Parent
};

struct Parent {
   std::string name;
   std::vector<std::shared_ptr<Child>> children;    // Parent owns its Children
};

//**************************************************************************
// Print the name of a child's parent, if the parent still exists.
// lock() yields a temporary shared_ptr, or an empty one if the Parent
// has already been destroyed.
//**************************************************************************
void show_parent(const Child &c)
{
   if (auto p = c.parent.lock()) {
      printf("%s's parent is %s\n", c.name.c_str(), p->name.c_str());
   } else {
      printf("%s's parent is gone\n", c.name.c_str());
   }
}

//**************************************************************************
// Build a Parent with one Child, then destroy the Parent while the Child lives on.
//**************************************************************************
int main()
{
   auto dad = std::make_shared<Parent>();
   dad->name = "Dad";
   auto kid = std::make_shared<Child>();
   kid->name = "Kid";
   kid->parent = dad;            // weak: does not add to dad's use_count
   dad->children.push_back(kid);

   show_parent(*kid);            // Kid's parent is Dad
   dad.reset();                  // last owner gone, so Parent is destroyed
   show_parent(*kid);            // Kid's parent is gone
   return 0;
}