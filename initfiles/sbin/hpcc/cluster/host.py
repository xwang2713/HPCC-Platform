'''
/*#############################################################################

    HPCC SYSTEMS software Copyright (C) 2012 HPCC Systems.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
############################################################################ */
'''

import subprocess
import socket
import re

class Host(object):
    '''
    This class represent cluster host attributions. Currently only ip is required.
    Several static help methods are provided to create cluster hast list.
    '''
    # Default user name and passwords
    user_name = 'hpcc'
    user_passowrd = 'hpcc'
    admin_password = 'hpcc'

    def __init__(self, ip=None):
        '''
        Constructor
        '''
        self._ip = ip
        self._host_name = None
        self._user_name = None
        self._user_password = None
        self._admin_password = None

    @property
    def ip(self):
        return self._ip

    @ip.setter
    def ip(self, value):
        self._ip = value

    @property
    def host_name(self):
        return self._host_name

    @host_name.setter
    def host_name(self, value):
        self._host_name = value

    @property
    def user_name(self):
        if self._user_name:
            return self._user_name
        else:
            return Host.user_name

    @user_name.setter
    def user_name(self, value):
        self._user_name = value


    @property
    def user_password(self):
        if self._user_password:
            return self._user_password
        else:
            return Host.user_password

    @user_password.setter
    def user_password(self, value):
        self._user_password = value



    @property
    def admin_password(self):
        if self._admin_password:
            return self._admin_password
        else:
            return Host.admin_password

    @admin_password.setter
    def admin_password(self, value):
        self._admin_password = value


    @staticmethod
    def get_hosts_from_env(env_xml="/etc/HPCCSystems/environment.xml", hpcc_home="/opt/HPCCSystems", exclude_local=False):
        cmd = hpcc_home + "/sbin/configgen -env " + env_xml + \
              " -machines | awk -F, '{print $1} ' | sort | uniq"

        hosts = []
        try:
            ips = subprocess.check_output(cmd, shell=True)
            for ip in ips.split():
                ip = ip.strip()
                if ip:
                    hosts.append( Host(ip) )
        except subprocess.CalledProcessError:
            print "Cannot get ip list from " + env_xml

        if exclude_local:
            return Host.exclude_local_host(hosts)
        else:
            return hosts

    @staticmethod
    def get_hosts_from_file(file_name,exclude_local=False):
        hosts = []
        with open(file_name) as host_file:
            for line in host_file:
                ip = line.strip()
                if ip:
                    hosts.append( Host(ip) )
        if exclude_local:
            return Host.exclude_local_host(hosts)
        else:
            return hosts

    @staticmethod
    def exclude_local_host(in_hosts):
        out_hosts = []
        addr_list = socket.getaddrinfo(socket.gethostname(), None)
        for host in in_hosts:
            found = False
            for addr in addr_list:
                if addr[4][0] == host.ip:
                    found = True
                    break
            if not found:
                out_hosts.append(host)

        #The above does not work if hostname doesn't match ip
        out_hosts_2 = []
        cmd = '/sbin/ifconfig -a | grep \"[[:space:]]*inet[[:space:]]\"'
        inet_out = subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True)
        if not inet_out:
            return out_hosts

        for host in out_hosts:
            m = re.search(host.ip, inet_out)
            if not m:
                out_hosts_2.append(host)

        return out_hosts_2

if __name__ == '__main__':
    #hosts = Host.get_hosts_from_env()
    hosts = Host.get_hosts_from_file('/Users/ming/work/python/host.lst')
    print "There are " + str(len(hosts)) + " hpcc nodes"
    for host in hosts:
        print host.ip