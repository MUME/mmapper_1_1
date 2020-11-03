Name: mmapper
Summary: MMapper - MUME (fire.pvv.org 4242) graphic mapper.
Version: 0.7.mdk
Release: 1
License: Freeware
Group: Applications/Internet
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/build-root-%{name}
AutoReqProv: yes
Packager: M. Krejza
Distribution: Mandrake
Prefix: /usr
Url: http://mmapper.czechian.net

%description
MMapper is an graphical MUD mapper specially designed for
MUME online game, which is available at fire.pvv.org
(port 4242). It works as proxy, which stands betwen game
server and text based client (telnet or any other mud
client with some extra functionality).

%prep
rm -rf $RPM_BUILD_ROOT 
mkdir $RPM_BUILD_ROOT

%setup -q

%build
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" \
./configure --prefix=%{prefix}
make -j 2

%install
make DESTDIR=$RPM_BUILD_ROOT install-strip

cd $RPM_BUILD_ROOT


find . -type d -fprint $RPM_BUILD_DIR/file.list.%{name}.dirs
find . -type f -fprint $RPM_BUILD_DIR/file.list.%{name}.files.tmp
sed '/\/man\//s/$/.gz/g' $RPM_BUILD_DIR/file.list.%{name}.files.tmp > $RPM_BUILD_DIR/file.list.%{name}.files
find . -type l -fprint $RPM_BUILD_DIR/file.list.%{name}.libs
sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' $RPM_BUILD_DIR/file.list.%{name}.dirs > $RPM_BUILD_DIR/file.list.%{name}
sed 's,^\.,\%attr(-\,root\,root) ,' $RPM_BUILD_DIR/file.list.%{name}.files >> $RPM_BUILD_DIR/file.list.%{name}
sed 's,^\.,\%attr(-\,root\,root) ,' $RPM_BUILD_DIR/file.list.%{name}.libs >> $RPM_BUILD_DIR/file.list.%{name}

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf $RPM_BUILD_DIR/file.list.%{name}
rm -rf $RPM_BUILD_DIR/file.list.%{name}.libs
rm -rf $RPM_BUILD_DIR/file.list.%{name}.files
rm -rf $RPM_BUILD_DIR/file.list.%{name}.files.tmp
rm -rf $RPM_BUILD_DIR/file.list.%{name}.dirs

%files -f ../file.list.%{name}
%defattr(-,root,root,0755)
