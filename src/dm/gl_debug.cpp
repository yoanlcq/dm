#include <dm/dm.hpp>

using namespace std;

namespace dm {

#ifdef DM_GL_DEBUG

static void GLAPIENTRY gl_dbg_glDebugMessageCallback_dummy(GLDEBUGPROC callback, 
                                       const void *userParam) {}
PFNGLDEBUGMESSAGECALLBACKPROC gl_dbg_glDebugMessageCallback = 
                              gl_dbg_glDebugMessageCallback_dummy;

static void GLAPIENTRY gl_dbg_glDebugMessageControl_dummy(GLenum source, GLenum type, 
                                      GLenum severity, GLsizei count, 
                                      const GLuint *ids, 
                                      GLboolean enabled) {}
PFNGLDEBUGMESSAGECONTROLPROC gl_dbg_glDebugMessageControl =
                             gl_dbg_glDebugMessageControl_dummy;

static void GLAPIENTRY gl_dbg_glDebugMessageInsertAMD(GLenum source, GLenum type, 
                                    GLuint id, GLenum severity, GLsizei length, 
                                    const char *message) 
{
    GLenum category;
    switch(source) {
    case GL_DEBUG_SOURCE_API: 
        category = GL_DEBUG_CATEGORY_API_ERROR_AMD;
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: 
        category = GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD;
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: 
        category = GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD;
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        category = GL_DEBUG_CATEGORY_APPLICATION_AMD;
        break;
    default:
        switch(type) {
        case GL_DEBUG_TYPE_ERROR: 
            category = GL_DEBUG_CATEGORY_API_ERROR_AMD;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: 
            category = GL_DEBUG_CATEGORY_DEPRECATION_AMD;
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: 
            category = GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD;
            break;
        case GL_DEBUG_TYPE_PERFORMANCE: 
            category = GL_DEBUG_CATEGORY_PERFORMANCE_AMD;
            break;
        default: category = GL_DEBUG_CATEGORY_OTHER_AMD; break;
        }
        break;
    }

    if(severity==GL_DEBUG_SEVERITY_NOTIFICATION)
        severity = GL_DEBUG_SEVERITY_LOW_AMD;

    glDebugMessageInsertAMD(category, severity, id, length, message);
}
static void GLAPIENTRY gl_dbg_glDebugMessageInsert_dummy(GLenum source, GLenum type, GLuint id, 
                                    GLenum severity, GLsizei length, 
                                    const char *message) {}
PFNGLDEBUGMESSAGEINSERTPROC gl_dbg_glDebugMessageInsert =
                            gl_dbg_glDebugMessageInsert_dummy;

static GLuint GLAPIENTRY gl_dbg_glGetDebugMessageLog_dummy(GLuint count, GLsizei bufSize, 
                                     GLenum *sources, GLenum *types, 
                                     GLuint *ids, GLenum *severities,
                                     GLsizei *lengths, 
                                     GLchar *messageLog) { return 0; }
PFNGLGETDEBUGMESSAGELOGPROC gl_dbg_glGetDebugMessageLog =
                            gl_dbg_glGetDebugMessageLog_dummy;

static void GLAPIENTRY gl_dbg_glPushDebugGroup_dummy(GLenum source, GLuint id, GLsizei length, 
                                const char *message) {}
PFNGLPUSHDEBUGGROUPPROC gl_dbg_glPushDebugGroup =
                        gl_dbg_glPushDebugGroup_dummy;

static void GLAPIENTRY gl_dbg_glPopDebugGroup_dummy(void) {}
PFNGLPOPDEBUGGROUPPROC gl_dbg_glPopDebugGroup =
                       gl_dbg_glPopDebugGroup_dummy;

static void GLAPIENTRY gl_dbg_glObjectLabel_dummy(GLenum identifier, GLuint name, GLsizei length, 
                             const char *label) {}
PFNGLOBJECTLABELPROC gl_dbg_glObjectLabel =
                     gl_dbg_glObjectLabel_dummy;

static void GLAPIENTRY gl_dbg_glObjectPtrLabel_dummy(const void *ptr, GLsizei length, 
                                const GLchar *label) {}
PFNGLOBJECTPTRLABELPROC gl_dbg_glObjectPtrLabel =
                        gl_dbg_glObjectPtrLabel_dummy;

static void GLAPIENTRY gl_dbg_glGetObjectLabel_dummy(GLenum identifier, GLuint name, 
                                GLsizei bufSize,
                                GLsizei *length, char *label) {}
PFNGLGETOBJECTLABELPROC gl_dbg_glGetObjectLabel =
                        gl_dbg_glGetObjectLabel_dummy;

static void GLAPIENTRY gl_dbg_glGetObjectPtrLabel_dummy(const void *ptr, GLsizei bufSize, 
                                   GLsizei *length, char *label) {}
PFNGLGETOBJECTPTRLABELPROC gl_dbg_glGetObjectPtrLabel =
                           gl_dbg_glGetObjectPtrLabel_dummy;


/* The good news is that the ARB constants have the same value as their
 * core alias. That is, GL_DEBUG_SOURCE_API == GL_DEBUG_SOURCE_API_ARB. 
 *
 * So we can use this callback for both the KHR and ARB setups.
 * However we still get to write another version of this callback for
 * the AMD setup.
 */
void GLAPIENTRY gl_dbg_gl_debug_msg_callback(GLenum source, GLenum type, 
                                GLuint id, GLenum severity, 
                                GLsizei length, const GLchar* message, 
                                const void* userParam)
{
    string source_str="";
    switch(source) {
    case GL_DEBUG_SOURCE_API: 
        source_str = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: 
        source_str = "Window system"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: 
        source_str = "Shader compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: 
        source_str = "3rd party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:
        source_str = "Application"; break;
    case GL_DEBUG_SOURCE_OTHER:
        source_str = "Other"; break;
    }
    string type_str="";
    switch(type) {
    case GL_DEBUG_TYPE_ERROR: 
        type_str = "Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: 
        type_str = "Deprecated behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: 
        type_str = "Undefined behaviour"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: 
        type_str = "Performance"; break;
    case GL_DEBUG_TYPE_PORTABILITY:
        type_str = "Portability"; break;
    case GL_DEBUG_TYPE_MARKER:
        type_str = "Command stream annotation"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        type_str = "Push debug group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:
        type_str = "Pop debug group"; break;
    case GL_DEBUG_TYPE_OTHER:
        type_str = "Other"; break;
    }
    string severity_str="";
    switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH:         severity_str = "High"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       severity_str = "Medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          severity_str = "Low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "Info"; break;
    }

    cerr << "OpenGL debug message "
         << "(severity:" << severity_str << ", type:" << type_str 
         << ", source:" << source_str 
         << ", id:" << hex << "0x" << id << dec << ") :"
         << endl << string(message) << endl << "---" << endl;
}

void GLAPIENTRY gl_dbg_gl_debug_msg_callback_amd(GLuint id, GLenum category,
                                GLenum severity, 
                                GLsizei length, const GLchar* message, 
                                void* userParam)
{
    string category_str="";
    switch(category) {
    case GL_DEBUG_CATEGORY_API_ERROR_AMD: 
        category_str = "API Error";
        break;
    case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
        category_str = "Window system";
        break;
    case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
        category_str = "Deprecated behaviour";
        break;
    case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
        category_str = "Undefined behaviour";
        break;
    case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
        category_str = "Performance";
        break;
    case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
        category_str = "Shader compiler";
        break;
    case GL_DEBUG_CATEGORY_APPLICATION_AMD:
        category_str = "Application";
        break;
    case GL_DEBUG_CATEGORY_OTHER_AMD:
        category_str = "Other";
        break;
    }
    string severity_str="";
    switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH_AMD:   severity_str = "High"; break;
    case GL_DEBUG_SEVERITY_MEDIUM_AMD: severity_str = "Medium"; break;
    case GL_DEBUG_SEVERITY_LOW_AMD:    severity_str = "Low"; break;
    }

    cerr << "OpenGL debug message "
         << "(severity:" << severity_str
         << ", category:" << category_str 
         << ", id:" << hex << "0x" << id << dec << ") :"
         << endl << string(message) << endl << "---" << endl;
}

struct fgl_ign_message {
    GLenum source;
    GLenum type;
    GLuint id;
};
typedef struct fgl_ign_message fgl_ign_message;

static const fgl_ign_message fgl_ign_messages[] = {
    /* From the NVidia driver, this message keeps popping up when 
     * calling glClear() while GL_SCISSOR_TEST is enabled. */
    { GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, 0x020043 },
    /* A null id indicates the end of the array. */
    { GL_DONT_CARE, GL_DONT_CARE, 0 }
};

struct fgl_ign_message_amd {
    GLenum category;
    GLuint id;
};
typedef struct fgl_ign_message_amd fgl_ign_message_amd;

static const fgl_ign_message_amd fgl_ign_messages_amd[] = {
    /* Fill here as you wish, but please keep the last element. */
    /* A null id indicates the end of the array. */
    { 0, 0 }
};

static void fgl_dbg_reset_funcs(void) {
#define HELPER(glFoo) \
    gl_dbg_##glFoo = gl_dbg_##glFoo##_dummy
    HELPER(glDebugMessageCallback); 
    HELPER(glDebugMessageControl);
    HELPER(glDebugMessageInsert); 
    HELPER(glGetDebugMessageLog); 
    HELPER(glPushDebugGroup);
    HELPER(glPopDebugGroup);
    HELPER(glObjectLabel);
    HELPER(glObjectPtrLabel); 
    HELPER(glGetObjectLabel); 
    HELPER(glGetObjectPtrLabel);
#undef HELPER
}

static void fgl_dbg_setup_khr(bool enable) {

    if(!enable) {
        glDisable(GL_DEBUG_OUTPUT);
        fgl_dbg_reset_funcs();
        return;
    } 

    const fgl_ign_message *ignmsg;
    
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_dbg_gl_debug_msg_callback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 
                          0, NULL, GL_TRUE);
    for(ignmsg = fgl_ign_messages ; ignmsg->id ; ++ignmsg) {
        glDebugMessageControl(ignmsg->source, ignmsg->type, GL_DONT_CARE,
                              1, &(ignmsg->id), GL_FALSE);
    }
#define HELPER(glFoo) \
    gl_dbg_##glFoo = glFoo
    HELPER(glDebugMessageCallback);
    HELPER(glDebugMessageControl);
    HELPER(glDebugMessageInsert);
    HELPER(glGetDebugMessageLog);
    HELPER(glPushDebugGroup);
    HELPER(glPopDebugGroup);
    HELPER(glObjectLabel);
    HELPER(glObjectPtrLabel);
    HELPER(glGetObjectLabel);
    HELPER(glGetObjectPtrLabel);
#undef HELPER
}

static void fgl_dbg_setup_arb(bool enable) {

    if(!enable) {
        fgl_dbg_reset_funcs();
        return;
    } 

    const fgl_ign_message *ignmsg;
    
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glDebugMessageCallbackARB(gl_dbg_gl_debug_msg_callback, NULL);
    glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 
                          0, NULL, GL_TRUE);
    for(ignmsg = fgl_ign_messages ; ignmsg->id ; ++ignmsg) {
        glDebugMessageControlARB(ignmsg->source, ignmsg->type, GL_DONT_CARE,
                              1, &(ignmsg->id), GL_FALSE);
    }
#define HELPER(glFoo) \
    gl_dbg_##glFoo = glFoo##ARB
    HELPER(glDebugMessageCallback);
    HELPER(glDebugMessageControl);
    HELPER(glDebugMessageInsert);
    HELPER(glGetDebugMessageLog);
#undef HELPER
    if(gl_has(EXT_debug_label)) {
        gl_dbg_glObjectLabel = glLabelObjectEXT;
        gl_dbg_glGetObjectLabel = glGetObjectLabelEXT;
    } else {
        gl_dbg_glObjectLabel = gl_dbg_glObjectLabel_dummy;
        gl_dbg_glGetObjectLabel = gl_dbg_glGetObjectLabel_dummy;
    }
#define HELPER(glFoo) \
    gl_dbg_##glFoo = gl_dbg_##glFoo##_dummy
    HELPER(glPushDebugGroup);
    HELPER(glPopDebugGroup);
    HELPER(glObjectPtrLabel);
    HELPER(glGetObjectPtrLabel);
#undef HELPER
}

static void fgl_dbg_setup_amd(bool enable) {

    fgl_dbg_reset_funcs();

    if(!enable)
        return;

    const fgl_ign_message_amd *ignmsg;

    glDebugMessageCallbackAMD(gl_dbg_gl_debug_msg_callback_amd, NULL);
    glDebugMessageEnableAMD(0, 0, 0, NULL, GL_TRUE);
    
    for(ignmsg = fgl_ign_messages_amd ; ignmsg->id ; ++ignmsg)
        glDebugMessageEnableAMD(ignmsg->category,0, 1, &(ignmsg->id),GL_FALSE);
    
    gl_dbg_glDebugMessageInsert = gl_dbg_glDebugMessageInsertAMD;
    
    if(gl_has(EXT_debug_label)) {
        gl_dbg_glObjectLabel = glLabelObjectEXT;
        gl_dbg_glGetObjectLabel = glGetObjectLabelEXT;
    }
}

static void fgl_dbg_insert_marker_ext(const char *string) {
    glInsertEventMarkerEXT(0, string);
}
static void fgl_dbg_insert_marker_gremedy(const char *string) {
    glStringMarkerGREMEDY(0, string);
}

#endif /* DM_GL_DEBUG */

static void fgl_dbg_insert_marker_dummy(const char *string) {}
void (*gl_debug_insert_marker)(const char *string) = fgl_dbg_insert_marker_dummy;

void gl_debug_setup(GLint major, GLint minor, bool enable) {
#ifdef DM_GL_DEBUG
    bool khr_supported = gl_has(KHR_debug) || major>4 || (major==4 && minor>=3);
    if(khr_supported)
        fgl_dbg_setup_khr(enable);
    else {
        if(gl_has(ARB_debug_output))
            fgl_dbg_setup_arb(enable);
        else if(gl_has(AMD_debug_output))
            fgl_dbg_setup_amd(enable);
        else enable = false;
    }
    if(gl_has(EXT_debug_marker))
        gl_debug_insert_marker = fgl_dbg_insert_marker_ext;
    else if(gl_has(GREMEDY_string_marker))
        gl_debug_insert_marker = fgl_dbg_insert_marker_gremedy;
    else
        gl_debug_insert_marker = fgl_dbg_insert_marker_dummy;
#else
    enable = false;
#endif
    cout << "OpenGL debug is " << (enable ? "enabled" : "disabled") 
         << "." << endl;
}

} // namespace dm
