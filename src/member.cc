#include "member.h"
#include <climits>

#include <queue>

#include "database.h"

namespace algdb {

void Member::DumpConnections() {
  std::cout << "(" << member_id << ")";
  for (auto &m : connecting_members) {
    auto conn = m.second;
    std::cout << "->" << conn.dst->member_id << 
      "(" << conn.group->group_id << ")";
  }
  std::cout << "\n";
}

void Member::PathToMemberBFS(uint64_t dst_member_id) {
    this->color = COLOR_GRAY;
    this->parent = NULL;
    std::queue<Member*> q;
    q.push(this);
    while (!q.empty()){
      Member* m = q.front();
      q.pop();
      for (auto mm : m->connecting_members){
        auto memcon = mm.second;
        auto v = memcon.dst;
        if(v->member_id == dst_member_id){
          v->parent = m;
          PrintPath(v);
          return;
        }
        if (v->color == COLOR_WHITE){
          v->color = COLOR_GRAY;
          v->parent = m;
          q.push(v);
        }
      }
      m->color = COLOR_BLACK;
  } 
}
void Member::PathToMemberIDDFS(uint64_t dst_member_id) {
  for (int depth = 0; depth < INT_MAX; depth++){
    Member *found = DLS(this, depth, dst_member_id);
    if (found != NULL){
      PrintPath(found);
      return;
    }
  }
}

Member *Member::DLS(Member *node, int depth, uint64_t dst_member_id){
  if (depth == 0 && node->member_id == dst_member_id){
    return node;
  }
  if(depth >0){
    for(auto child : node->connecting_members){
      auto memcon = child.second;
      auto found = DLS(memcon.dst, depth -1, dst_member_id);
      if (found != NULL){
        memcon.dst->parent = node;
        return found;
      }
    }
  }
  return NULL;
}
  
void Member::PrintPath(Member* dst) {
  Member *curr = dst;
  while(curr && curr != this) {
    std::cout << curr->member_name << " <- ";
    curr = curr->parent;
  }
  std::cout << this->member_name << "\n";
}

}
