list(APPEND zsearch_sources
	${CMAKE_CURRENT_LIST_DIR}/baseIndex.cpp
	${CMAKE_CURRENT_LIST_DIR}/fixedIndex.cpp
	${CMAKE_CURRENT_LIST_DIR}/variableIndex.cpp
	${CMAKE_CURRENT_LIST_DIR}/indexManager.cpp
	${CMAKE_CURRENT_LIST_DIR}/indexRepository.cpp
	${CMAKE_CURRENT_LIST_DIR}/importFile.cpp
	${CMAKE_CURRENT_LIST_DIR}/importFileBase.cpp
	${CMAKE_CURRENT_LIST_DIR}/importTextFile.cpp
	${CMAKE_CURRENT_LIST_DIR}/importLZ4File.cpp
	${CMAKE_CURRENT_LIST_DIR}/dataImporter.cpp
	${CMAKE_CURRENT_LIST_DIR}/indexChunk.cpp
	${CMAKE_CURRENT_LIST_DIR}/datajoin.cpp
)