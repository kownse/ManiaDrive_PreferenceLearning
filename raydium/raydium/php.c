/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/php.h"
#endif

// PHP support for Raydium
// Known bug: recursive Ray/PHP calls are fatal (segfault in zend core).

#include "php_wrappers.h"

// Dirty globals... (needed for WIN32 PHP support)
#ifdef ZTS
zend_compiler_globals *compiler_globals;
zend_executor_globals *executor_globals;
php_core_globals *core_globals;
sapi_globals_struct *sapi_globals;
void ***tsrm_ls;
#endif


static int sapi_raydium_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static void sapi_raydium_send_header(sapi_header_struct *sapi_header, void *server_context TSRMLS_DC)
{
}


static int php_dummy(sapi_module_struct *sapi_module)
{
return SUCCESS;
}

static void sapi_raydium_register_variables(zval *track_vars_array TSRMLS_DC)
{
// Here for future reference:
//php_register_variable("NAME", "value", vars_array TSRMLS_CC);
}

void raydium_php_error(int type, const char *msg, ...)
{
raydium_log("^cERROR type %i",type);
}

int raydium_php_uwrite(const char *str, uint str_length TSRMLS_DC)
{
 if(str_length>=RAYDIUM_MAX_NAME_LEN-15)
    {
    raydium_log("PHP output is too long, redirected to stdout");
    printf("%s",str);
    }
 else
    raydium_log("%s",str);
 return SUCCESS;
}

// Ref: main/SAPI.h in PHP source tree
static sapi_module_struct raydium_sapi_module =
{
        "RayHandler",                   /* name */
        "Raydium PHP Handler",          /* pretty name */

// Since PHP@Win32 is dirty... :
        php_dummy,             /* startup */
        php_dummy,             /* shutdown */

        NULL,                           /* activate */
        NULL,                           /* deactivate */

        raydium_php_uwrite,             /* unbuffered write */
        NULL,                           /* flush */
        NULL,                           /* get uid */
        NULL,                           /* getenv */

        raydium_php_error,              /* error handler */

        NULL,                           /* header handler */
        sapi_raydium_send_headers,      /* send headers handler */
        sapi_raydium_send_header,       /* send header handler */

        NULL,                           /* read POST data */
        NULL,                           /* read Cookies */

        sapi_raydium_register_variables,/* register server variables */
        NULL,                           /* Log message */
        NULL,                           /* Get Request Time */

        NULL,                           /* INI Path (changed later) */

        NULL,                           /* Block interruptions */
        NULL,                           /* Unblock interruptions */

// 243:         void (*default_post_reader)(TSRMLS_D);
        NULL,
// 244:         void (*treat_data)(int arg, char *str, zval *destArray TSRMLS_DC);
        NULL,
// 245:         char *executable_location;
        NULL,
// 246:
// 247:         int php_ini_ignore;
        0,
// 248:
// 249:         int (*get_fd)(int *fd TSRMLS_DC);
        NULL,
// 250:
// 251:         int (*force_http_10)(TSRMLS_D);
        NULL,
// 252:
// 253:         int (*get_target_uid)(uid_t * TSRMLS_DC);
        NULL,
// 254:         int (*get_target_gid)(gid_t * TSRMLS_DC);
        NULL,
// 255:
// 256:         unsigned int (*input_filter)(int arg, char *var, char **val, unsigned int val_len, unsigned int *new_val_len TSRMLS_DC);
        NULL,
// 257:
// 258:         void (*ini_defaults)(HashTable *configuration_hash);
        NULL,
// 259:         int phpinfo_as_text;
        1,
        NULL    /* char *ini_entries; */
};


void raydium_php_init_request(char *filename)
{
//Must allocate memory for those 3 ones :
//SG(request_info).request_method = "GET";
//SG(request_info).query_string = "";
//SG(request_info).content_type = "text/html";

SG(request_info).request_method = NULL;
SG(request_info).query_string = NULL;
SG(request_info).content_type = NULL;
SG(request_info).request_uri = filename;
SG(request_info).path_translated = filename;
SG(request_info).content_length = 0;
SG(sapi_headers).http_response_code = 200;
SG(server_context) = NULL;
}


int raydium_php_exec(char *name)
{
    FILE *fp;
    char suffix[32];
    zval *vars[RAYDIUM_MAX_REG_VARIABLES];
    zend_file_handle file_handle;
    zend_llist global_vars;
    int i,nvars;


    // Do not use rayphp auto-downloading for *.php files
    raydium_file_ext(suffix,name);
    if(strcasecmp(suffix,"php"))
    {
        fp=raydium_file_fopen(name,"rb");
        if(!fp)
            {
            raydium_log("php: ERROR: cannot pre-open '%s' file",name);
            return 0;
            }
        fclose(fp);
    }


// PHP_ MAJOR and MINOR _VERSION **seems** to be introduced with PHP 4.3
#ifdef PHP_MAJOR_VERSION
    // >= 4.3
    if(php_module_startup(&raydium_sapi_module,NULL,0) == FAILURE)
#else
    if(php_module_startup(&raydium_sapi_module) == FAILURE)
#endif
    {
        return FAILURE;
    }
    raydium_php_init_request(name);

    zend_llist_init(&global_vars, sizeof(char *), NULL, 0);
    zend_alter_ini_entry("html_errors", 12, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
    zend_alter_ini_entry("max_execution_time", 19, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
    zend_alter_ini_entry("default_socket_timeout",23,"10",2, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
/*
    file_handle.type = ZEND_HANDLE_FILENAME;
    file_handle.filename = SG(request_info).path_translated;
    file_handle.free_filename = 0;
    file_handle.opened_path = NULL;
*/

    file_handle.handle.fp=VCWD_FOPEN(name,"rb");
    if(!file_handle.handle.fp)
        {
        raydium_log("php: ERROR: cannot post-open '%s' file",name);
        return 0;
        }
    file_handle.filename=name;
    file_handle.type = ZEND_HANDLE_FP;
    file_handle.free_filename = 0;
    file_handle.opened_path = NULL;


//    if(php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC) == FAILURE) {
    if(php_request_startup(TSRMLS_C) == FAILURE) {
        php_module_shutdown(TSRMLS_C);
        return FAILURE;
    }

    SG(headers_sent) = 1;
    SG(request_info).no_headers = 1;

 // i save "raydium_register_variable_index" here since it may change during script exec
 nvars=raydium_register_variable_index;
 for(i=0;i<nvars;i++)
    {
    if(raydium_register_variable_type[i]==RAYDIUM_REGISTER_INT)
        {
        MAKE_STD_ZVAL(vars[i]); // init
        ZVAL_LONG(vars[i],(*(int *)raydium_register_variable_addr[i]));
        ZEND_SET_SYMBOL(&EG(symbol_table), raydium_register_variable_name[i], vars[i]);
        }

    if(raydium_register_variable_type[i]==RAYDIUM_REGISTER_SCHAR)
        {
        MAKE_STD_ZVAL(vars[i]); // init
        ZVAL_LONG(vars[i],(int)(*(char *)raydium_register_variable_addr[i]));
        ZEND_SET_SYMBOL(&EG(symbol_table), raydium_register_variable_name[i], vars[i]);
        }

    if(raydium_register_variable_type[i]==RAYDIUM_REGISTER_FLOAT)
        {
        MAKE_STD_ZVAL(vars[i]); // init
        ZVAL_DOUBLE(vars[i],(*(float *)raydium_register_variable_addr[i]));
        ZEND_SET_SYMBOL(&EG(symbol_table), raydium_register_variable_name[i], vars[i]);
        }

    if(raydium_register_variable_type[i]==RAYDIUM_REGISTER_STR)
        {
        MAKE_STD_ZVAL(vars[i]); // init
        ZVAL_STRING(vars[i],(char *)raydium_register_variable_addr[i],1); // 1 means "duplicate string"
        ZEND_SET_SYMBOL(&EG(symbol_table), raydium_register_variable_name[i], vars[i]);
        }

    if(raydium_register_variable_type[i]==RAYDIUM_REGISTER_ICONST)
        {
        zend_register_long_constant(raydium_register_variable_name[i],
                                    strlen(raydium_register_variable_name[i])+1,
                                    (*(int *)raydium_register_variable_addr[i]),
                                    CONST_CS,
                                    0 TSRMLS_CC);
        }

    if(raydium_register_variable_type[i]==RAYDIUM_REGISTER_FCONST)
        {
        zend_register_double_constant(raydium_register_variable_name[i],
                                      strlen(raydium_register_variable_name[i])+1,
                                      (*(float *)raydium_register_variable_addr[i]),
                                      CONST_CS,
                                      0 TSRMLS_CC);
        }
    }
#ifdef WIN32
 zend_register_functions(NULL, raydium_register_function_list,CG(function_table), MODULE_PERSISTENT,TSRMLS_C);
 php_execute_script(&file_handle,TSRMLS_C);
#else
 zend_register_functions(NULL, raydium_register_function_list,CG(function_table), MODULE_PERSISTENT);
 php_execute_script(&file_handle CLS_CC ELS_CC PLS_CC);
#endif

 // now get back variable values
 for(i=0;i<nvars;i++)
    {
        if(raydium_register_variable_type[i]==RAYDIUM_REGISTER_INT)
         {
            if(vars[i]->type == IS_LONG)
                *(int *)raydium_register_variable_addr[i]=vars[i]->value.lval;
            else raydium_log("php: (int)%s type have changed ! Cannot read new value.",raydium_register_variable_name[i]);
         }

        if(raydium_register_variable_type[i]==RAYDIUM_REGISTER_SCHAR)
         {
            if(vars[i]->type == IS_LONG)
                *(char *)raydium_register_variable_addr[i]=vars[i]->value.lval;
            else raydium_log("php: (int)%s type have changed ! Cannot read new value.",raydium_register_variable_name[i]);
         }

        if(raydium_register_variable_type[i]==RAYDIUM_REGISTER_FLOAT)
         {
            if(vars[i]->type == IS_DOUBLE)
                *(float *)raydium_register_variable_addr[i]=vars[i]->value.dval;
            else if(vars[i]->type == IS_LONG)
                *(float *)raydium_register_variable_addr[i]=vars[i]->value.lval;
            else raydium_log("php: (float)%s type have changed ! Cannot read new value.",raydium_register_variable_name[i]);
         }

        if(raydium_register_variable_type[i]==RAYDIUM_REGISTER_STR)
         {
            if(vars[i]->type == IS_STRING)
                strcpy((char*)raydium_register_variable_addr[i],vars[i]->value.str.val);
            else if(vars[i]->type == IS_DOUBLE)
                sprintf((char*)raydium_register_variable_addr[i],"%f",vars[i]->value.dval);
            else if(vars[i]->type == IS_LONG)
                sprintf((char*)raydium_register_variable_addr[i],"%li",vars[i]->value.lval);
            else raydium_log("php: (char *)%s type have changed ! Cannot read new value.",raydium_register_variable_name[i]);
         }
    }

 php_request_shutdown(NULL);
 raydium_sapi_module.shutdown(&raydium_sapi_module);
 php_module_shutdown(TSRMLS_C);
 return 1;
}


void raydium_php_close(void)
{
sapi_shutdown();
}

void raydium_php_init(void)
{
char *path;
#ifdef ZTS
tsrm_startup(1, 1, 0, NULL);
compiler_globals = (zend_compiler_globals*)ts_resource(compiler_globals_id);
executor_globals = (zend_executor_globals*)ts_resource(executor_globals_id);
core_globals = (php_core_globals*)ts_resource(core_globals_id);
sapi_globals = (sapi_globals_struct*)ts_resource(sapi_globals_id);
tsrm_ls = (void***)ts_resource(0);
#endif
//raydium_sapi_module.phpinfo_as_text=1;
path=(char*)malloc(strlen(PHP_INI_PATH)+1);
strcpy(path,PHP_INI_PATH);
raydium_sapi_module.php_ini_path_override=path;
sapi_startup(&raydium_sapi_module);
raydium_atexit(raydium_php_close);

if(raydium_php_rayphp_path[0]==0) // raydium_php_rayphp_path_change() ?
    raydium_init_cli_option_default("rayphp",raydium_php_rayphp_path,RAYPHP_PATH);

// Linux default
if(!raydium_file_isdir(raydium_php_rayphp_path) &&
    raydium_file_isdir("rayphp"))
		strcpy(raydium_php_rayphp_path,"rayphp");

// win32 SDK default
if(!raydium_file_isdir(raydium_php_rayphp_path) &&
    raydium_file_isdir("../raydium/rayphp"))
		strcpy(raydium_php_rayphp_path,"../raydium/rayphp");

if(!raydium_file_isdir(raydium_php_rayphp_path))
	{
	raydium_log("*** Error: PHP R3S scripts not found !");
	raydium_log("*** Search was: '%s', './rayphp' and '../raydium/rayphp'",raydium_php_rayphp_path);
	exit(10);
	}

raydium_log("PHP R3S scripts path: %s",raydium_php_rayphp_path);
raydium_log("PHP support: OK");
}
