# put-CAwLLP-ini
Repo containing the task for the 2nd project of Computer Architecture with Low-Level Programming - parsing ini files

Requirements & Specs:
Main goal - C program to parse INI files using only C standard library

Requirements & Specs:
- (3.0) accept two command line parameters in format `$ ./program PATH-TO-INI-FILE.ini section.key`
- (3.0) print out value under a key in `[data]` section
- (3.0) assume limits: `no. of sections`, `len of section name`, `len of key`, `len of value` < 30
  (4.5) these limits are not valid, can be any length (no compile-time limit)
- (3.5) detect missing section and print out dedicated messagee, e.g. `Failed to find section [data]`
- (3.5) detect missing key in a present section, e.g. `Failed to find key "key" in section [data]`
- (4.0) parse the data into well defined structures, e.g. `struct section { ... };`
- (4.0) detect invalid identifiers in INI file (section name and keys), i.e. those that contain char other than letters and digits
- (5.0) distinguish types of values between strings and numbers
- (5.0) understand simple expressions given by given command line parameters (`+, -, *, /` for numbers and `+ (concatenation)` for strings)
- (5.0) expressions with operrands of diff types or invalid operators are invalid
