#pragma once

#define LANG_NAME       "PickleLisp"

#define READ_BUF_LEN    64              // initial read buffer length (dynamically doubles when exhausted)
#define ERR_BUF_LEN     512             // maximum allowed length of error message
#define REPL_IN         "8=> "
#define EXTENSION       ".pickle"       // pickle scripts extension
#define EUPSILON        1e-6            // precision of the equality assertion between doubles
