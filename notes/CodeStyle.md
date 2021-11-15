## Code Style

* indentation: 2 spaces (no tabs)
* opening brace generally at trailing end of line
* remove trailing spaces - ide can usually handle this
* variable names usually reference their type
* variable names generally start with lower case
* class variables have m_ prefix (not always followed in structs, but mostly)
* Classes and Methods are generally capitalized
* recent code has _t suffix for maps, vectors, set definitions, and other various composite structures
* includes generally follow this pattern:
  * system libraries first, 
  * third part libraries next, 
  * my own libraries, 
  * directory local libraries
* most of the time, the includes are in christmas tree format
* strive for readable code, rather than try to use kinky koding
  styles
* use of strategic TODO and NOTE entries are desireable
* comment strategically to improve readability and understanding
  * I find that coming back several days later is the optimal time to insert comments in strategic places
* when re-ordering chunks of code or methods, use separate commits to aid traceability
* enum is preferred over #define
* in comparisons, the constant should come first so as to help compile time catch silliness such as '=' vs '=='
