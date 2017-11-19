FROM centos:7

MAINTAINER CodeLibs Project

RUN yum -y groupinstall base "Development tools" --setopt=group_package_types=mandatory,default,optional
RUN yum clean all

RUN mkdir /work
WORKDIR /work

CMD ["sh", "/opt/bin/run.sh"]

