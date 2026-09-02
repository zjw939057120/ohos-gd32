# GD32F470 快速上手 OpenHarmony

---

## 目录

- [开源鸿蒙开发指南](#开源鸿蒙开发指南)
  - [开发环境配置](#开发环境配置)
  - [源码获取](#源码获取)
  - [编译构建](#编译构建)

---

## 开源鸿蒙开发指南

### 开发环境配置

#### 安装依赖库

> **注意**：以下命令适用于 Ubuntu 22.04 系统，其他发行版请根据实际情况调整。

安装Python 3.9 及其相关依赖：

```bash
sudo apt install software-properties-common -y
sudo add-apt-repository ppa:deadsnakes/ppa -y
sudo apt install python3.9 python3.9-distutils -y

```

将默认 Python 版本设置为 3.9：

```bash
sudo rm /usr/bin/python
sudo ln -s /usr/bin/python3.9 /usr/bin/python

```

安装编译所需依赖：

```bash
sudo apt-get update && sudo apt-get install \
  libstdc++-12-dev binutils binutils-dev git git-lfs gnupg flex bison gperf \
  build-essential zip curl zlib1g-dev gcc-multilib libc6-dev-i386 \
  x11proto-core-dev libx11-dev ccache libgl1-mesa-dev libxml2-utils \
  xsltproc unzip m4 bc gnutls-bin python3-pip ruby genext2fs \
  device-tree-compiler make libffi-dev e2fsprogs pkg-config perl \
  openssl libssl-dev libelf-dev libdwarf-dev u-boot-tools mtd-utils \
  cpio doxygen liblz4-tool openjdk-8-jre gcc g++ texinfo dosfstools \
  mtools default-jre default-jdk libncurses5 apt-utils wget scons tar \
  rsync git-core libxml2-dev grsync xxd libglib2.0-dev libpixman-1-dev \
  kmod jfsutils reiserfsprogs xfsprogs squashfs-tools pcmciautils \
  quota ppp libtinfo-dev libtinfo5 libstdc++6 gcc-arm-none-eabi vim \
  ssh locales libxinerama-dev libxcursor-dev libxrandr-dev libxi-dev
```

#### 配置 Shell 环境

确保系统默认 Shell 为 `bash`：

```bash
# 检查当前 Shell
ls -l /bin/sh
# 若输出不是 bash，执行以下命令进行修改
sudo dpkg-reconfigure dash
# 在弹出的界面中选择 "No"，将默认 Shell 改为 bash

```

#### 安装 repo 工具（首次使用）

```bash

# 创建 OpenHarmony 目录
mkdir -p /mnt/sda/OpenHarmony-4.1-Release/OpenHarmony
cd /mnt/sda/OpenHarmony-4.1-Release/OpenHarmony

# 创建 ohos 目录
mkdir -p ~/ohos
curl https://raw.gitcode.com/gitcode-dev/repo/raw/main/repo-py3 > ~/ohos/repo
chmod a+x ~/ohos/repo
pip3 install -i https://repo.huaweicloud.com/repository/pypi/simple requests

```

***

### 源码获取

> **提示**：源码体积较大（约 15GB），编译器及二进制工具（约 3.7GB），请确保磁盘空间充足，并保持网络连接稳定。

#### 获取 OpenHarmony 源码

```bash
# 初始化 repo
~/ohos/repo init -u https://gitcode.com/openharmony/manifest -b OpenHarmony-4.1-Release -g ohos:mini --no-repo-verify

# 同步代码
~/ohos/repo sync -c --force-sync -j16
~/ohos/repo forall -c 'git lfs pull' -v -j16

# 安装编译器及二进制工具
./build/prebuilts_download.sh

# 安装hb工具
sudo python -m pip install build/hb

# 删除原始的 device 和 vendor 目录
sudo rm -rf device vendor

# 复制工程到当前目录
cp -r ../ohos-gd32-OpenHarmony-4.1-Release/* .

```

### 编译构建

```bash
# 选择产品配置
make gd32f470zi_config

# 编译
make -j16

```

