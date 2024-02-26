FROM debian:stable

ARG TOOLCHAIN_TARBALL=Wine_MSVC_VS2005_drive_c.tar.gz
ARG DEBIAN_FRONTEND=noninteractive

RUN dpkg --add-architecture i386 \
&& apt-get update \
&& apt-get install -y --no-install-recommends wine wine64 wine32:i386 winbind \
&& apt-get clean

ENV WINEPREFIX=/root/.wine
RUN wineboot --init && wineserver --wait

ADD https://comrade.one/$TOOLCHAIN_TARBALL /
RUN tar -xaf /$TOOLCHAIN_TARBALL -C /root/.wine/drive_c/ && rm /$TOOLCHAIN_TARBALL
