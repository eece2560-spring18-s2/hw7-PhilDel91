# Homework 7 STL and Graph Algorithms

## Overview

In this homework, we are going to practice how to implement graph-related algorithms. At the same time, 
we are going to practice the use of C++ STL.

## Using STL

Since we have introduced the C++ standard template library (STL) in class, we are going to use STL to replace 
all the containers implemented in the previous homeworks. We favor the STL containers than self-implemented 
containers because
i) they are very stable, ii) they are highly optimized for performance, and iii) they are richer in features.

You may notice that the `container` folder has been fully removed from this homework. The `Database` class is 
fully upgraded with STL containers. We use three `std::vector`s to represent the Group, Member, and Event table. 
We also  create two indices as `group_id_hash_index` and `member_id_hash_index` using the container 
`std::unordered_map` which provides the STL implementation of the HashTable.

The `Member` class and the `Group` class have also been updated to map the new changes. As we are going to 
create a graph of members according to who join the same group, we added a few vectors to the classes. In 
`Member` class, we have a `group_ids` field telling the ids of the groups that the `Member` is in. The `groups` 
vector holds the pointer to all the groups that the `Member` is in.  Similarly, the `Group` class has a `members`
vector field and it should hold all the members that are registered in the `Group`. The content of the vectors 
is filled by function `Database::AssociateMembersWithGroups`.

Due to the limitation of the memory available, we will not be able to hold the whole member graph on the table. 
We added 3 parameters to the function `Database::LoadData` function. These 3 integer parameters limit the number
of events, members, and groups to be loaded from the tables. When you test your code, you can play with the 
limitations to see if everything works correct or not with a small-scale data.

The existing code serves as a good example of how to use the STL
containers.  If you need more information about `vector` and `unordered_map`, you can refer to the documentation of
[vector](http://www.cplusplus.com/reference/vector/vector/) and
[unordered_map](http://www.cplusplus.com/reference/unordered_map/unordered_map/)
online.

## P1. Building the graph (3 pt.)

In this problem, we are going to build a graph of members. If two members are in the same graph, we consider 
that there is an edge that is connecting the two members. We represent the graph with an adjacency list. In the 
`Member` class, there is a field called `connecting_members`. It is a hash table that maps from
the destination member id to a `MemberConnection`. A `MemberConnection` only has two fields, the group that 
links the two members and the destination member of the connection.

You will need to implement the function of `Database::BuildMemberGraph`. This function should take the `members`
vector and the `groups` vector as input and after the function is called, all the members' `connecting_members` 
vectors are properly filled. Note that when you add an edge, you need to add the two members to each other's
`connecting_members` field. Also, if two members share more than one group, there should only be one edge added, 
you can only add the first group you iterate on.

In your report, briefly describe your solution. Analysis the algorithm
complexity with the big O notation. Explain how you get the result. You can assume that there are G groups, 
M members. In average, each group has K members, each member joins L groups, and each member is connected to 
B other members. You do not have to use all the letters here. If necessary, define your own variables.

## P2. Connecting to someone using Breath-first Search (2 pt.)

Suppose you are a member of meetup.com and you want to know another member who is also on meetup.com. You can
only get to know that member of your friends who join the same group as you and your friends' friend. 

In this question, implement the function `Member::PathToMemberBFS` with BFS algorithm. This function finds the
path from `this` member to the member with the id of provided `dst_member_id` argument.

## P3. Connecting to someone using IDDFS (3 pt.)

In P3, you need to complete the same task, but with the IDDFS algorithm.

Benchmark the main program for p2 and p3. Report the times that each algorithm consumes. Also, measure the peak 
memory consumption of the two algorithms.  You can first run the main program with p2 and p3 without calling 
the pathfinding functions and see what is the memory cost. And then run p2 and p3 with the pathfinding functions.
Subtracting the peak memory consumption of p2 and p3 from the memory consumption when you run p2 or p3 without the
path-finding function should give you the real memory consumption of the pathfinding functions. To get the peak 
memory consumption, run the following command 

```bash
/usr/bin/time -v ./p2_main
```

The output field called `Maximum resident set size` is the peak memory consumption you are looking for. In 
your report explain your finding.

## P4. Find best Groups to join (2 pt.)

In P4, suppose you are not a member of meetup.com and you want to join meetup.com to extend your network. 
You want to know everyone in a certain community. So you need to select certain groups to join. Since large 
groups provide fewer opportunities for you to talk to a certain person, you prefer the groups with less number
of members.

In this problem, you need to implement `Database::BestGroupsToJoin()` function. This function takes an
argument of type `Member *` as a member who you know that is in the community. You need to implement the
with Prim's algorithm, treating the provided `Member` as the root of the minimum spanning tree you are 
going to generate. The weight of a connection is represented as the number of members in the group plus 1, 
as implemented at `member.h:26`. The function `BestGroupsToJoin` returns the total weight of the connections
in your spanning tree. If your spanning tree includes two connections that map to the same meetup.com groups,
you should treat it as two different groups. The `BestGroupsToJoin` need to configure the parent pointers in 
the `Member` class and returns the total weight of your minimum spanning tree. After the function is called, 
I should be able to print the path from the root member to any other members.
