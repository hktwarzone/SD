/*
challenges/simple-database
*/

#include <iostream>
#include <vector>
#include <map>
#include <unordered_set>
#include <stack>

using namespace std;

class DataBase {
private:
   // two maps: one for key->value, one for value->key
   // each key records its previous uncommited values in a stack (O(1) push/pop)
   map<string, stack<string>> keyToValue;
   // for value->key map, unordered_set<string> is used (O(1) find/remove)
   map<string, unordered_set<string>> valueToKey;
   // a stack for tracking open transaction blocks
   stack<vector<string>> transaction;
public:

   // SET name value – Set the variable name to the value value. Neither variable names nor values will contain spaces
   void set_command(string name, string value) {
      // erase the previous value of "name" from the value->key map
      if (keyToValue.find(name) != keyToValue.end() && (!keyToValue[name].empty() && keyToValue[name].top() != ""))
         valueToKey[keyToValue[name].top()].erase(name);
      // add "name -> value" entry to the first map, or just modify the top entry if no opened transaction block
      if(transaction.empty() && !keyToValue[name].empty())
         keyToValue[name].top() = value;
      else
         keyToValue[name].push(value);
      // add "value -> name" to the second map
      if (value != "")
         valueToKey[value].insert(name);
      // record "name" in this transaction
      if (!transaction.empty())
         transaction.top().push_back(name);
   }

   // GET name – Print out the value of the variable name, or NULL if that variable is not set.
   void get_command(string name) {
      if(keyToValue.find(name) != keyToValue.end() && (!keyToValue[name].empty() && keyToValue[name].top() != ""))
         cout << keyToValue[name].top() << endl;
      else
         cout << "NULL" << endl;
   }

   // UNSET name – Unset the variable name, making it just like that variable was never set.
   void unset_command(string name) {
      set_command(name, "");
   }

   // NUMEQUALTO value – Print out the number of variables that are currently set to value.
   // If no variables equal that value, print 0.
   void numequalto_command(string value) {
      // v->k map tracks the variables with the same value
      if(valueToKey.find(value) != valueToKey.end()) {
         cout << valueToKey[value].size() << endl;
      }
      else cout << "0" << endl;
   }

   // BEGIN – Open a new transaction block.
   void begin_command() {
      transaction.push(vector<string>());
   }

   // ROLLBACK – Undo all of the commands issued in the most recent transaction block, and close the block.
   void rollback_command() {
      if (transaction.empty())
         cout << "NO TRANSACTION" << endl;
      else {
         // get all variables that have been modified in the most recent transaction
         vector<string> most_recent_trans = transaction.top();
         for (auto var = most_recent_trans.begin(); var != most_recent_trans.end(); var++) {
            if (keyToValue.find(*var) != keyToValue.end() && !keyToValue[*var].empty()) {
               // erase the most recent value of variable from both maps
               valueToKey[keyToValue[*var].top()].erase(*var);
               keyToValue[*var].pop();
               // restore the previous value of variable to v->k map
               if (!keyToValue[*var].empty() && keyToValue[*var].top() != "") {
                  valueToKey[keyToValue[*var].top()].insert(*var);
               }
            }
         }
         // close the transaction block
         transaction.pop();
      }
   }

   // COMMIT – Close all open transaction blocks, permanently applying the changes made in them.
   void commit_command() {
      if (transaction.empty())
         cout << "NO TRANSACTION" << endl;
      else {
         // close all open transaction blocks
         transaction = stack<vector<string>>();
         // all previous values of variables can be deleted
         for(auto var = keyToValue.begin(); var != keyToValue.end(); var++) {
            if (var->second.size() > 1) {
               string latest = var->second.top();
               var->second = stack<string>();
               if (latest != "")
                  var->second.push(latest);
            }
         }
      }
   }
};

class IOHelper {
public:
   // helper function for c++ string tokenization with blank delimiter
   vector<string> tokenize(string& s) {
      vector<string> output;
      if (s.empty()) return output;
      string token;
      for(int i = 0; i < s.size(); i++) {
         if (s[i] != ' ')
            token += s[i];
         else {
            if (!token.empty())
               output.push_back(token);
            token = "";
         }
      }
      if (!token.empty())
         output.push_back(token);
      return output;
   }

   // exception handler for invalid command
   void exception_handler(string s) {
      if (s == "SET")
         cout << "Usage: SET <name> <value>" << endl;
      else if (s == "GET")
         cout << "Usage: GET <name>" << endl;
      else if (s == "UNSET")
         cout << "Usage: UNSET <name>" << endl;
      else if (s == "NUMEQUALTO")
         cout << "Usage: NUMEQUALTO <value>" << endl;
      else
         cout << "Valid Command: SET GET UNSET NUMEQUALTO BEGIN ROLLBACK COMMIT END" << endl;
   }
};

int main() {
   string commandline;
   DataBase db;
   IOHelper io;

   while(getline(cin, commandline)) {
      vector<string> args = io.tokenize(commandline);
      if (args.empty())
         io.exception_handler("");
      else if (args[0] == "END")
         break;
      else if (args[0] == "BEGIN")
         db.begin_command();
      else if (args[0] == "ROLLBACK")
         db.rollback_command();
      else if (args[0] == "COMMIT")
         db.commit_command();
      else if (args[0] == "SET" && args.size() >= 3)
         db.set_command(args[1], args[2]);
      else if (args[0] == "GET" && args.size() >= 2)
         db.get_command(args[1]);
      else if (args[0] == "UNSET" && args.size() >= 2)
         db.unset_command(args[1]);
      else if (args[0] == "NUMEQUALTO" && args.size() >= 2)
         db.numequalto_command(args[1]);
      else
         io.exception_handler(args[0]);
   }
   return 0;
}
