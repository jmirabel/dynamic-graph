# ---------------------------------------------------------------
# --- SOURCE FILE NAMES -----------------------------------------
# ---------------------------------------------------------------
SET(SOT_MODULE_NAME dgraph)

# --- Add your Source Files here.
SET(libdg_${SOT_MODULE_NAME}_src
      dgraph/pool.cpp
      dgraph/interpreter.cpp
      dgraph/factory.cpp
      dgraph/entity.cpp
      dgraph/plugin-loader.cpp  
)

SET(libdg_${SOT_MODULE_NAME}_templates "")
SET(libdg_${SOT_MODULE_NAME}_headers "")

SET(libdg_${SOT_MODULE_NAME}_plugins_headers "")
SET(libdg_${SOT_MODULE_NAME}_plugins_templates "")
SET(libdg_${SOT_MODULE_NAME}_plugins_src 
      dgraph/shell-functions.cpp
      dgraph/shell-procedure.cpp
)

# ---------------------------------------------------------------
# --- GENERIC RULES ---------------------------------------------
# ---------------------------------------------------------------
SOT_SRC_SUBDIR_PROCESS()