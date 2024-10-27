#pragma once

#define ERR_BUF_LEN     512             // maximum allowed length of error message
#define READ_BUF_LEN    64              // initial read buffer length (dynamically doubles when exhausted)
#define REPL_IN         "8=> "
#define LANG_NAME       "PickleLisp"
#define EXTENSION       ".pickle"       // pickle scripts extension
#define STD_LIB_PATH    "../stdlib/std.pickle"