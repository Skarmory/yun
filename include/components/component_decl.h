#ifndef YUN_COMPONENTS_COMPONENT_DECL_H
#define YUN_COMPONENTS_COMPONENT_DECL_H

#define DECL_COMPONENT(argname)\
    extern struct string* g_##argname##_name;\
    extern int g_##argname##_id;\
    struct argname

#define COMPONENT_ID(argname) g_##argname##_id

#define DECL_COMPONENT_NAME_ID(argname)\
    struct string* g_##argname##_name = NULL;\
    int g_##argname##_id = -1

#define INIT_COMPONENT(argname)\
    do{\
    g_##argname##_name = string_new(#argname);\
    g_##argname##_id = string_hash(g_##argname##_name);\
    component_type_register(g_##argname##_name, sizeof(struct argname));\
    } while(0)

#define UNINIT_COMPONENT(argname)\
    do{\
        string_free(g_##argname##_name);\
    } while(0)

#endif
