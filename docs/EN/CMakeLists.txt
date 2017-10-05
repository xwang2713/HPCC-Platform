################################################################################
#    HPCC SYSTEMS software Copyright (C) 2012 HPCC Systems®.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
################################################################################

###
#  This CMakeLists intended to be used in docs/EN
#    Refactored for internationalization
##
#GHACKED THIS 
# Doc dirs to include.
add_subdirectory(DynamicESDL)
add_subdirectory(ECLLanguageReference)
add_subdirectory(ECLProgrammersGuide)
add_subdirectory(ECLStandardLibraryReference)
add_subdirectory(ECLReference)
add_subdirectory(EclipseHelp)
add_subdirectory(HTMLHelp)
add_subdirectory(HPCCClientTools)
add_subdirectory(HPCCCertify)
add_subdirectory(HPCCDataHandling)
add_subdirectory(HPCCDataTutorial)
add_subdirectory(HPCCMonitoring)
add_subdirectory(HPCCSystemAdmin)
add_subdirectory(IMDB)
add_subdirectory(InstantCloud)
add_subdirectory(Installing_and_RunningTheHPCCPlatform)
add_subdirectory(RoxieReference)
add_subdirectory(RunningHPCCinaVirtualMachine)
add_subdirectory(ECLPlayground)
add_subdirectory(ECLPluginForEclipse)
add_subdirectory(ECLScheduler)
add_subdirectory(ECLWatch)
add_subdirectory(VisualizingECL)


# Docs automation
if (DOCS_AUTO)
   add_subdirectory(XMLGeneration)
   add_subdirectory(UsingConfigManager)
endif()


#WIP -  Docs
#add_subdirectory(RuningHPCCinAmazonWebServicesEC2)

add_subdirectory(PortalHTML)

GET_PROPERTY(_targets GLOBAL PROPERTY DOC_TARGETS)
add_custom_target(docs ALL DEPENDS ${_targets})
