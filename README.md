# ush
 ### Description:
 Ucode project: develop Unix shell. 
 ### Developers:
 [Maksym Libovych](https://github.com/mlibovych) | 
 [Serhiy Nikolayenko](https://github.com/N-911) | 
 [Tetiana Rohalska](https://github.com/trohalska)
 
 ### Usage:
 ```
 1. make
 2 ./ush
 ```
 ### Implemented:
 **1. Builtins:**
 ```
 - export
 - unset
 - exit
 - fg [%n, %str]
 - env [-i, -P, -u]
 - cd [-s, -P, -]
 - pwd [-L, -P]
 - which [ -a, -s]
 - echo [-n ,-e , -E]
 - jobs
 - bd
 - true, false
 - kill
 - chdir
 ```
 **2. Manage signals:**
 ```
 CTRL+D
 CTRL+C
 CTRL+Z
 ```
 **3. Expansions:**
 ```
 tilde expansion ~
 ${parameter}, $parameter
 command substitution `command` and $(command)
 ``` 
 **4. Other:**
 * customize prompt;
 * command editing using arrow-keys;
 * support of:
    * commands history;
    * shell functions;
    * pipes | ;
    * & ;
    * redirecting <, >, >> ;
    * logical operators && and || ;
    * aliases.
