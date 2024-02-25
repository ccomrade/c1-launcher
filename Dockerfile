FROM debian:stable

ARG TOOLCHAIN_TARBALL=Wine_MSVC_VS2005_drive_c.tar.gz
ARG TOOLCHAIN_TARBALL_SHA256=4558c89e8acc90b09a0d0ce9036efb74fb369fc448397ff565534b51eb295b6f

ARG DEBIAN_FRONTEND=noninteractive

RUN dpkg --add-architecture i386 \
&& apt-get update \
&& apt-get install -y --no-install-recommends wine wine64 wine32:i386 winbind \
&& apt-get clean

ADD --checksum=sha256:$TOOLCHAIN_TARBALL_SHA256 https://comrade.one/$TOOLCHAIN_TARBALL /
RUN tar -xaf /$TOOLCHAIN_TARBALL -C /root/.wine/drive_c/ && rm $TOOLCHAIN_TARBALL

RUN wineboot --init && wineserver --wait
