#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

/*
   Operations:
   Move forward in the same line: 1
   Move backward in the same line: 2
   Move to the beginning of another line: 3
   Move to the end of another line: 4
 */

 struct openvec {
   int fscore;
   int gscore;
   int hscore;
   int id;
   int comefrom;
   vector <string> grid;
 };

int heuristic(vector <string> init, vector <string> goal, int lane_number, int locations, string car){
        //Here we introduce the initial and goal position and calculate the heuristic (minimum cost from init to goal)
        int h;
        int init_lane;
        int init_column;
        int goal_lane;
        int goal_column;

        for (int i = 0; i < lane_number; i++) {
                for (int j = 0; j < locations; j++) {
                        if (init[i*locations+j]==car) {
                                init_lane=i;
                                init_column=j;
                                break;
                        }
                }
        }
        for (int i = 0; i < lane_number; i++) {
                for (int j = 0; j < locations; j++) {
                        if (goal[i*locations+j]==car) {
                                goal_lane=i;
                                goal_column=j;
                                break;
                        }
                }
        }

        h = abs(init_lane-goal_lane)+abs(init_column-goal_column);
        return h;
}

//This function calculates the movement cost for each non-blocked car
int cost(vector <string> parking, int lane_number, int loc, int initial_lane_number, int initial_loc, int locations){
  int checker = 0;
        if(lane_number==initial_lane_number) { //Same lane
                if(loc>initial_loc) { //Final location bigger than initial
                  for(int i = initial_loc+1; i<loc; i++){
                    if(parking[lane_number*locations+i]!="__"){ //Check if blocked for that location
                      if(loc==locations-1){ //If location is final one, check the other way
                        for(int j = initial_loc-1; j>-1; j--){
                          if(parking[lane_number*locations+j]!="__"){
                              return 5;
                            }
                        }
                        return 4;
                      }
                        return 5;
                      }
                    }
                    return 1; //Possible grid -> forward in same lane
                  }
                else if(loc<initial_loc){
                  for(int i = initial_loc-1; i>loc; i--){
                    if(parking[lane_number*locations+i]!="__"){
                      if(loc==0){
                        for(int j = initial_loc+1; j<locations; j++){
                          if(parking[lane_number*locations+j]!="__"){ //Failure over here
                            return 5;
                          }
                        }
                        return 3;
                      }
                        return 5;
                      }
                  }
                  return 2; //Possible grid -> backwards in same lane
                }
        }
        else{
          //Check if a final position on its line can be reached
          for(int i = initial_loc+1; i<locations; i++){
            if (parking[initial_lane_number*locations+i]!="__"){
              checker = 1; //Set one if it is blocked by this side
            }
          }
          for(int i = initial_loc-1; i>-1; i--){
            if (parking[initial_lane_number*locations+i]!="__"){
              if(checker==1){ //If blocked by one side and also blocked for this, set checker equal two
                checker=2;
              }
            }
          }
          if(checker==2){ //If blocked by two sides, no possible grid
            return 5;
          }
          else{
            if(loc==0) { //Possible grid -> first position of another lane
                    return 3;
            }
            else if(loc==locations-1) { //Possible grid -> last position of another lane
                    return 4;
            }
          }
        }
        return 5;
}


void astar(vector <string> init_parking_mat, vector <string> goal_parking_mat, int lane_number, int locations){

        //struct vector <<closedvec> > closed_set;
        vector <openvec> open_set;
        vector <openvec> closed_set;
        open_set.push_back(openvec());
        open_set[0].grid = init_parking_mat;
        open_set[0].id = 0;
        int minimum = 0; //Minimum fscore found
        int hscore = 0; //Heuristic of the node
        int id_counter = 0; //Number of ids generated by the algorithm

        for (int i = 0; i < lane_number; i++) {
          for (int j = 0; j < locations; j++) {
            hscore += heuristic(init_parking_mat, goal_parking_mat, lane_number, locations, init_parking_mat[i*locations+j]);
          }
        }

        open_set[0].hscore = hscore;
        open_set[0].gscore = 0;
        open_set[0].fscore = open_set[0].hscore + open_set[0].gscore;

        cout << "The fscore is: " << open_set[0].fscore << endl;


         while(open_set.size()!=0){
           struct openvec current_vec;
           //Here we look in the open_set list to find the grid with minimum fcost
           for (int i = 0; i < open_set.size(); i++) {
             if(open_set.size()==1){
               //If only the origin is open, we automatically asign it to current_vec
               if(open_set[i].id==0){
                 current_vec.id = i;
                 current_vec.fscore = open_set[i].fscore;
                 minimum = current_vec.fscore;
                 current_vec.grid = open_set[i].grid;
               }
             }
             if(open_set[i].fscore<minimum){
               current_vec.id = i;
               current_vec.fscore = open_set[i].fscore;
               minimum = current_vec.fscore;
               current_vec.grid = open_set[i].grid;
             }
           }

           for(int i = 0; i<open_set.size(); i++){
             if(open_set[i].id==current_vec.id){
               open_set.erase(open_set.begin()+i);
             }
           }

           closed_set.push_back(current_vec);

           //Now we have the node with minimum cost
           vector <openvec> decendents;
           for(int i = 0; i<lane_number; i++){
             for(int j = 0; j<locations; j++){
               for(int k = 0; k<lane_number; k++){
                 for(int l = 0; l<locations; l++){
                   if(current_vec.grid[i*locations+j]!="__" && current_vec.grid[k*locations+l]=="__"){
                     if(cost(current_vec.grid, k, l, i, j, locations)!=5){
                       struct openvec node;
                       node.id = id_counter++;
                       node.gscore = cost(current_vec.grid, k, l, i, j, locations);
                       int auxscore = 0;
                       node.grid = current_vec.grid;
                       node.grid[k*locations+l]=node.grid[i*locations+j];
                       node.grid[i*locations+j]="__";
                       for (int x = 0; x < lane_number; x++) {
                         for (int y = 0; y < locations; y++) {
                           auxscore += heuristic(current_vec.grid, goal_parking_mat, lane_number, locations, init_parking_mat[x*locations+y]);
                         }
                       }
                       node.hscore = auxscore;
                       node.fscore = node.hscore + node.gscore;
                       decendents.push_back(node);
                       for (int w = 0; w < lane_number; w++) {
                               for (int v = 0; v < locations; v++) {
                                       cout << node.grid[w*locations+v] << " ";
                               }
                               cout << endl;
                       }
                       cout << endl;
                     }
                   }
                 }
               }
             }
           }

           cout << decendents.size() << endl;

           for (int i = 0; i < decendents.size(); i++) {
             open_set.push_back(decendents[i]);
           }
         }

}

int main(int argc, char const *argv[]) {

        int counter = 0;
        vector<string> initialcars;
        string aux;

        //This part gets all strings from the initial state file
        ifstream initialfile(argv[1]);
        while (!initialfile.eof()) {
                initialfile >> aux;
                initialcars.push_back(aux);
        }
        initialfile.close();

        //Here we erase the last element of the vector, repeated due to the reading algorithm
        initialcars.pop_back();

        int lane_number = stoi(initialcars[0]);
        int locations = stoi(initialcars[1]);

        initialcars.erase(initialcars.begin());
        initialcars.erase(initialcars.begin());

        vector <string> init_parking_mat;

        for (int i = 0; i < lane_number; i++) {
                for (int j = 0; j < locations; j++) {
                        init_parking_mat.push_back(initialcars[0]);
                        initialcars.erase(initialcars.begin());
                }
        }

        //First check print
        for (int i = 0; i < lane_number; i++) {
                for (int j = 0; j < locations; j++) {
                        cout << init_parking_mat[i*locations+j] << " ";
                }
                cout << endl;
        }

        //Here, we get the data from the goal file

        vector<string> goalcars;

        ifstream goalfile(argv[2]);
        while (!goalfile.eof()) {
                goalfile >> aux;
                goalcars.push_back(aux);
        }
        goalfile.close();

        //Here we erase the last element of the vector, repeated due to the reading algorithm
        goalcars.pop_back();

        lane_number = stoi(goalcars[0]);
        locations = stoi(goalcars[1]);

        goalcars.erase(goalcars.begin());
        goalcars.erase(goalcars.begin());

        vector<string> goal_parking_mat;

        for (int i = 0; i < lane_number; i++) {
                for (int j = 0; j < locations; j++) {
                        goal_parking_mat.push_back(goalcars[0]);
                        goalcars.erase(goalcars.begin());
                }
        }

        // //Second check print
        // cout << endl;
        // for (int i = 0; i < lane_number; i++) {
        //         for (int j = 0; j < locations; j++) {
        //                 cout << goal_parking_mat[i*locations+j] << " ";
        //         }
        //         cout << endl;
        // }

        for (int i = 0; i < lane_number; i++) {
                for (int j = 0; j < locations; j++) {
                        if(init_parking_mat[i*locations+j]=="__") {
                        }
                        else{
                                counter++;
                        }
                }
        }

        if (counter==(lane_number*locations)) {
                cout << "Configuration unfeasible. No free slots." << endl;
                return 0;
        }

        astar(init_parking_mat, goal_parking_mat, lane_number, locations);

        // astar_search( init_parking_mat, goal_parking_mat, lane_number, locations);

        return 0;
}
