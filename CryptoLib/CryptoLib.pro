LANGUAGE = C++ 
TEMPLATE = lib 
MOC_DIR = Generated 
UI_DIR = Generated 
CONFIG += release qt rtti exceptions stl 
INCLUDEPATH += . .\Generated ..\QT\include ..\CryptoLib ..\UiCommon ..\HV_DLL ..\Alo ..\AloFilter ..\AloLegendImage ..\AloZoneModel ..\AloCmds ..\AloCmdsLim ..\AloCmdsTest ..\AloCmdsTestNew ..\AloDBData ..\AloDBMetadata ..\AloDBSymbology ..\AloRaster ..\AloCommon ..\AloDBAdmin ..\AloLocation ..\AloMetaData ..\AloClientContext ..\AloAttribute ..\AloSymbology ..\DbLayer ..\DbAdmin ..\TunnelMapCommon ..\Dime2\include ..\DrawingLib ..\DrawingQcLib ..\DrawingDxfLib ..\ConsistencyCheck ..\UpdateDb
TRANSLATIONS = CryptoLib_cs_CZ.ts CryptoLib_pt_PT.ts CryptoLib_de_GR.ts
QMAKE_LFLAGS += /NODEFAULTLIB 
DEFINES += NDEBUG 
LIBS += oldnames.lib odbccp32.lib msvcrt.lib msvcprt.lib Rpcrt4.lib HtmlHelp.lib 
DESTDIR = ..\Lib
TARGET = CryptoLib
OBJECTS_DIR = Release
CONFIG += warn_off

FORMS +=  

SOURCES += 3way.cpp adler32.cpp algebra.cpp algparam.cpp arc4.cpp asn.cpp base64.cpp basecode.cpp bfinit.cpp blowfish.cpp blumshub.cpp cast.cpp casts.cpp channels.cpp crc.cpp cryptlib.cpp default.cpp des.cpp dessp.cpp dh.cpp dh2.cpp diamond.cpp diamondt.cpp dsa.cpp ec2n.cpp eccrypto.cpp ecp.cpp elgamal.cpp eprecomp.cpp esign.cpp files.cpp filters.cpp fips140.cpp fipstest.cpp gf256.cpp gf2_32.cpp gf2n.cpp gfpcrypt.cpp gost.cpp gzip.cpp haval.cpp hex.cpp hrtimer.cpp ida.cpp idea.cpp integer.cpp iterhash.cpp luc.cpp mars.cpp marss.cpp md2.cpp md4.cpp md5.cpp md5mac.cpp misc.cpp modes.cpp modexppc.cpp mqueue.cpp mqv.cpp nbtheory.cpp network.cpp oaep.cpp osrng.cpp panama.cpp pch.cpp pkcspad.cpp polynomi.cpp pssr.cpp pubkey.cpp queue.cpp rabin.cpp randpool.cpp rc2.cpp rc5.cpp rc6.cpp rdtables.cpp rijndael.cpp ripemd.cpp rng.cpp rsa.cpp rw.cpp safer.cpp seal.cpp serpent.cpp sha.cpp shark.cpp sharkbox.cpp simple.cpp skipjack.cpp socketft.cpp square.cpp squaretb.cpp strciphr.cpp tea.cpp tftables.cpp tiger.cpp tigertab.cpp trdlocal.cpp twofish.cpp wait.cpp wake.cpp winpipes.cpp xtr.cpp xtrcrypt.cpp zdeflate.cpp zinflate.cpp zlib.cpp 

HEADERS += 3way.h adler32.h aes.h algebra.h algparam.h arc4.h argnames.h asn.h base64.h basecode.h blowfish.h blumshub.h cast.h cbcmac.h channels.h config.h crc.h cryptlib.h default.h des.h dh.h dh2.h diamond.h dmac.h dsa.h ec2n.h eccrypto.h ecp.h elgamal.h eprecomp.h esign.h files.h filters.h fips140.h fltrimpl.h gf256.h gf2_32.h gf2n.h gfpcrypt.h gost.h gzip.h haval.h hex.h hmac.h hrtimer.h ida.h idea.h integer.h iterhash.h lubyrack.h luc.h mars.h md2.h md4.h md5.h md5mac.h mdc.h misc.h modarith.h modes.h modexppc.h mqueue.h mqv.h nbtheory.h network.h nr.h oaep.h oids.h osrng.h panama.h pch.h pkcspad.h polynomi.h pssr.h pubkey.h pwdbased.h queue.h rabin.h randpool.h rc2.h rc5.h rc6.h rijndael.h ripemd.h rng.h rsa.h rw.h safer.h seal.h secblock.h seckey.h serpent.h sha.h shark.h simple.h skipjack.h smartptr.h socketft.h square.h strciphr.h tea.h tiger.h trdlocal.h trunhash.h twofish.h wait.h wake.h winpipes.h words.h xormac.h xtr.h xtrcrypt.h zdeflate.h zinflate.h zlib.h 
