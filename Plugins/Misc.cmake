function(target_install_interface TARGET)
	install(TARGETS "${TARGET}" EXPORT ${TARGET}Targets DESTINATION include)
	install(EXPORT ${TARGET}Targets
		FILE "${TARGET}Targets.cmake"
		NAMESPACE Ver::
		DESTINATION "lib/cmake/${TARGET}"
	)

	export(EXPORT ${TARGET}Targets
	   FILE "${TARGET}Targets.cmake"
	   NAMESPACE Ver::
	)
endfunction()