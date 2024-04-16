# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

# - Find Apache Portable Runtime
# Find the APR includes and libraries
# This module defines
#  APR_INCLUDE_DIR, where to find apr.h, etc.
#  APR_LIBRARIES, the libraries needed to use APR.
#  APR_FOUND, If false, do not try to use APR.
# also defined, but not for general use are
#  APR_LIBRARY, where to find the APR library.

FIND_PATH(APR_INCLUDE_DIR apr.h
   PATH_SUFFIXES apr-1 apr-1.0
)

SET(APR_NAMES ${APR_NAMES} libapr-1 apr-1)
FIND_LIBRARY(APR_LIBRARY
  NAMES ${APR_NAMES}
)

find_package_handle_standard_args(APR DEFAULT_MSG
   APR_LIBRARY APR_INCLUDE_DIR)

MARK_AS_ADVANCED(APR_INCLUDE_DIR APR_LIBRARY)
