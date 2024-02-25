FROM debian:stable

ARG TOOLCHAIN_TARBALL=Wine_MSVC_VS2005_drive_c.tar.gz

ARG DEBIAN_FRONTEND=noninteractive

RUN dpkg --add-architecture i386 \
&& apt-get update \
&& apt-get install -y --no-install-recommends wine wine64 wine32:i386 winbind \
&& apt-get clean

RUN adduser --disabled-password --gecos "" builder
USER builder

RUN wineboot --init && wineserver --wait

ADD --chown=builder:builder https://comrade.one/$TOOLCHAIN_TARBALL /tmp/
RUN tar -xaf /tmp/$TOOLCHAIN_TARBALL -C /home/builder/.wine/drive_c/ && rm /tmp/$TOOLCHAIN_TARBALL
