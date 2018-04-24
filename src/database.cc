#include "database.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <limits>
#include <queue>
#include <map>

#include "member.h"
#include "csv_reader.h"

namespace algdb {

Database::Database() {
}


Database::~Database() {
  for (auto event : events) {
    delete event; 
  }

  for (auto member : members) {
    delete member;
  }

  for (auto group : groups) {
    delete group;
  }
}

void Database::LoadEventData(const std::string &event_file, int limit) {
  CsvReader reader;
  reader.OpenFile(event_file.c_str());

  int count = 0;
  while(reader.NextEntry()) {
    count ++;
    if (count > limit) break;
    Event *event = new Event();
    for (int j = 0; j < 48; j++) {
      std::string cell = reader.NextCell();
      switch (j) {
      case 0:
        event->event_id = cell;
        break;
      case 3:
        event->duration = std::stoull(cell);
        break;
      case 6:
        event->fee = std::stoi(cell);
        break;
      case 14:
        event->group_id = std::stoull(cell);
        break;
      case 28:
        event->time = cell;
      case 47:
        event->yes_rsvp_count = std::stoi(cell);
        break;
      }
    }
    events.push_back(event);
  }
  std::cout << events.size() << " events loaded.\n";
}

void Database::LoadMemberData(const std::string &member_file, int limit) {
  CsvReader reader;
  reader.OpenFile(member_file.c_str());

  int count = 0;
  uint64_t last_id = 0;
  Member *member;

  while(reader.NextEntry()) {
    uint64_t id = std::stoull(reader.NextCell());
    if (id != last_id) {
      last_id = id;
      count++;
      if (count > limit) {
        break;
      }

      member = new Member();
      members.push_back(member);
      member->member_id = id;
      for (int j = 0; j < 13; j++) {
        std::string cell = reader.NextCell();
        switch (j) {
        case 5:
          member->lat = std::stod(cell);
          break;
        case 7:
          member->lon = std::stod(cell);
          break;
        case 8:
          member->member_name = cell;
          break;
        case 12:
          member->group_ids.push_back(std::stoull(cell));
        }
      }
      member_id_hash_index[member->member_id] = member;
    }
  }
  std::cout << members.size() << " merbers loaded.\n";
}

void Database::LoadGroupData(const std::string &group_file, int limit) {
  CsvReader reader;
  reader.OpenFile(group_file.c_str());

  int count = 0;
  while(reader.NextEntry()) {
    count++;
    if (count > limit) {
      break;
    }

    Group *group = new Group();
    for (int j = 0; j < 36; j++) {
      std::string cell = reader.NextCell();
      switch (j) {
      case 0:
        group->group_id = std::stoull(cell);
        break;
      case 20:
        group->group_name = cell;
        break;
      case 21:
        group->organizer_id = std::stoull(cell);
      case 29:
        group->rating = std::stof(cell);
        break;
      }
    }
    groups.push_back(group);
    group_id_hash_index[group->group_id] = group;
  }
  std::cout << groups.size() << " groups loaded.\n";
}

void Database::AssociateMembersWithGroups() {
  for (auto member : members) {
    for (auto group_id : member->group_ids) {

      // Check if the group_id is in the group_group_id_hash_index
      if (group_id_hash_index.find(group_id) != 
          group_id_hash_index.end()) {
        auto group = group_id_hash_index[group_id];
        member->groups.push_back(group);
        group->members.push_back(member);
      }
    }
  }
}

void Database::RandomizeGraph(int num_connections) {
  // Create some random connections to make the problem more interesting.
  srand(0);
  for (int i = 0; i < num_connections; i++) {
    int src_index = rand() % members.size();
    int dst_index = rand() % members.size();
    int group_index = rand() % groups.size();

    Member *src = members[src_index];
    Member *dst = members[dst_index];

    if (src == dst) continue;

    if (src->connecting_members.find(dst->member_id) == 
        src->connecting_members.end()) {
      MemberConnection conn;
      conn.group = groups[group_index];
      conn.dst = dst;
      src->connecting_members[dst->member_id] = conn;
      dst->connecting_members[src->member_id] = conn;
    }
  }
}


void Database::LoadData(const std::string &data_folder_path, 
                        int event_limit, 
                        int member_limit, 
                        int group_limit) {
  LoadEventData(data_folder_path + "/events.csv", event_limit);
  LoadMemberData(data_folder_path + "/members.csv", member_limit);
  LoadGroupData(data_folder_path + "/groups.csv", group_limit);
  AssociateMembersWithGroups();
  // RandomizeMembers();
}


void Database::BuildMemberGraph() {
  for (Group *g : groups){
    for (Member *m : g->members){
      for (Member *m2 : g->members){
        if(m == m2){
          continue;
        }
        if (m->connecting_members.find(m2->member_id) != m->connecting_members.end()){
          continue;
        }
        MemberConnection memcon;
        memcon.group = g;
        memcon.dst = m2;
        m->connecting_members [m2->member_id] = memcon;
      }
    }
  }

}

double Database::BestGroupsToJoin(Member *root) {
  double totalweight = 0;
  root->key = 0;
  std::vector<Member*>q;
  BuildMemberGraph();
  for (Member *m : members){
    if (m->member_id == root->member_id){
    }
    else{
      m->key = 9999999;
    }
    m->color=COLOR_WHITE;
    m->parent = NULL;
    q.push_back(m);
  }
  while(!q.empty()){
    Member* min = q.front();
    int min_index = 0;
    for(uint64_t n = 0; n < q.size(); n++){
      if(q[n]->key < min->key){
        min = q[n];
        min_index = n;
      }
    }
    q.erase(q.begin()+min_index);
    min->color=COLOR_BLACK;
    for(auto memcon : min->connecting_members){
      auto mc = memcon.second;
      auto v = mc.dst;
      
      if (v->color == COLOR_WHITE && mc.GetWeight() < v->key){
        v->key = mc.GetWeight();
        v->parent = min;
        totalweight = totalweight + v->key;
      }
    }
  }
  return totalweight;
}
}

