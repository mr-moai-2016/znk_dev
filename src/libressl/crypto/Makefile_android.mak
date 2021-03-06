# Source directory
S = .
MAKE_CMD=$(ZNK_NDK_DIR)/prebuilt/windows/bin/make
# Root path of common libraries
MY_LIBS_ROOT=..\..
ifndef MKFSYS_DIR
  MKFSYS_DIR=$(MY_LIBS_ROOT)\..\mkfsys
endif

ifndef MACHINE
  MACHINE=armeabi
endif
PLATFORM := android-$(MACHINE)

# Output directory
ABINAME := android-$(MACHINE)
O = .\out_dir\$(PLATFORM)
DLIBS_DIR=dlib\$(PLATFORM)
SLIBS_DIR=slib\$(PLATFORM)

PLATFORMS_LIST=$(wildcard $(ZNK_NDK_DIR)/platforms/android-*)
PLATFORMS_LEVEL=$(findstring $(ZNK_NDK_DIR)/platforms/android-9, $(PLATFORMS_LIST))
ifndef PLATFORMS_LEVEL
  PLATFORMS_LEVEL=$(word 1, $(PLATFORMS_LIST))
endif

ifeq ($(MACHINE), armeabi)
TOOLCHAINS_LIST=$(wildcard $(ZNK_NDK_DIR)/toolchains/arm-linux-androideabi-*/prebuilt/windows)
TOOLCHAINS_DIR=$(word $(words $(TOOLCHAINS_LIST)), $(TOOLCHAINS_LIST) )

COMPILER := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-gcc \
	-MMD -MP \
	-fpic -ffunction-sections -funwind-tables -fstack-protector -no-canonical-prefixes -march=armv5te -mtune=xscale -msoft-float -mthumb -Os \
	-g -DNDEBUG -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 \
	-DANDROID \
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM \
	-I$(PLATFORMS_LEVEL)/arch-arm/usr/include

LINKER   := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-g++ \
	--sysroot=$(PLATFORMS_LEVEL)/arch-arm \

RPATH_LINK := -Wl,-rpath-link=$(PLATFORMS_LEVEL)/arch-arm/usr/lib -Wl,-rpath-link=$O
LINKER_OPT := -no-canonical-prefixes  -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -mthumb -fPIE  -lc -lm
STRIP_UNNEEDED := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-strip --strip-unneeded
LIBAR := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-gcc-ar

endif
ifeq ($(MACHINE), armeabi-v7a)
TOOLCHAINS_LIST=$(wildcard $(ZNK_NDK_DIR)/toolchains/arm-linux-androideabi-*/prebuilt/windows)
TOOLCHAINS_DIR=$(word $(words $(TOOLCHAINS_LIST)), $(TOOLCHAINS_LIST) )

COMPILER := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-gcc \
	-MMD -MP \
	-fpic -ffunction-sections -funwind-tables -fstack-protector -no-canonical-prefixes -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp -mthumb -Os \
	-g -DNDEBUG -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 \
	-DANDROID \
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM \
	-I$(PLATFORMS_LEVEL)/arch-arm/usr/include \

LINKER   := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-g++ \
	--sysroot=$(PLATFORMS_LEVEL)/arch-arm \

RPATH_LINK := -Wl,-rpath-link=$(PLATFORMS_LEVEL)/arch-arm/usr/lib -Wl,-rpath-link=$O
LINKER_OPT := -no-canonical-prefixes -march=armv7-a -Wl,--fix-cortex-a8  -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -mthumb -fPIE  -lc -lm
STRIP_UNNEEDED := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-strip --strip-unneeded
LIBAR := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-gcc-ar

endif
ifeq ($(MACHINE), x86)
TOOLCHAINS_LIST=$(wildcard $(ZNK_NDK_DIR)/toolchains/x86-*/prebuilt/windows)
TOOLCHAINS_DIR=$(word $(words $(TOOLCHAINS_LIST)), $(TOOLCHAINS_LIST) )

COMPILER := $(TOOLCHAINS_DIR)/bin/i686-linux-android-gcc \
	-MMD -MP \
	-ffunction-sections -funwind-tables -no-canonical-prefixes -fstack-protector -O2 \
	-g -DNDEBUG -fomit-frame-pointer -fstrict-aliasing -funswitch-loops -finline-limit=300 \
	-DANDROID \
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM \
	-I$(PLATFORMS_LEVEL)/arch-x86/usr/include \

LINKER   := $(TOOLCHAINS_DIR)/bin/i686-linux-android-g++ \
	--sysroot=$(PLATFORMS_LEVEL)/arch-x86 \

RPATH_LINK := -Wl,-rpath-link=$(PLATFORMS_LEVEL)/arch-x86/usr/lib -Wl,-rpath-link=$O
LINKER_OPT := -no-canonical-prefixes  -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -fPIE  -lc -lm
STRIP_UNNEEDED := $(TOOLCHAINS_DIR)/bin/i686-linux-android-strip --strip-unneeded
LIBAR := $(TOOLCHAINS_DIR)/bin/i686-linux-android-gcc-ar

endif

CP=xcopy /H /C /Y
INCLUDE_FLAG+=  \
	-I../include \
	-I../include/compat \
	-I../crypto/asn1 \
	-I../crypto/bn \
	-I../crypto/evp \
	-I../crypto/modes \
	-I../crypto \


include Makefile_version.mak

BASENAME0=crypto
DLIB_NAME0=libcrypto-43.so
DLIB_FILE0=$O\$(DLIB_NAME0)
ILIB_FILE0=$O\libcrypto-43.so
SLIB_FILE0=$O\libcrypto.a
OBJS0=\
	$O\ts/ts_asn1.o \
	$O\ts/ts_req_utils.o \
	$O\ts/ts_rsp_sign.o \
	$O\ts/ts_conf.o \
	$O\ts/ts_rsp_print.o \
	$O\ts/ts_req_print.o \
	$O\ts/ts_err.o \
	$O\ts/ts_rsp_utils.o \
	$O\ts/ts_rsp_verify.o \
	$O\ts/ts_verify_ctx.o \
	$O\ts/ts_lib.o \
	$O\dsa/dsa_ossl.o \
	$O\dsa/dsa_sign.o \
	$O\dsa/dsa_gen.o \
	$O\dsa/dsa_meth.o \
	$O\dsa/dsa_lib.o \
	$O\dsa/dsa_depr.o \
	$O\dsa/dsa_prn.o \
	$O\dsa/dsa_err.o \
	$O\dsa/dsa_pmeth.o \
	$O\dsa/dsa_vrf.o \
	$O\dsa/dsa_ameth.o \
	$O\dsa/dsa_key.o \
	$O\dsa/dsa_asn1.o \
	$O\chacha/chacha.o \
	$O\ecdsa/ecs_vrf.o \
	$O\ecdsa/ecs_sign.o \
	$O\ecdsa/ecs_ossl.o \
	$O\ecdsa/ecs_err.o \
	$O\ecdsa/ecs_asn1.o \
	$O\ecdsa/ecs_lib.o \
	$O\rc2/rc2_skey.o \
	$O\rc2/rc2ofb64.o \
	$O\rc2/rc2cfb64.o \
	$O\rc2/rc2_ecb.o \
	$O\rc2/rc2_cbc.o \
	$O\whrlpool/wp_block.o \
	$O\whrlpool/wp_dgst.o \
	$O\x509v3/v3_ia5.o \
	$O\x509v3/v3_pcons.o \
	$O\x509v3/pcy_map.o \
	$O\x509v3/v3_pci.o \
	$O\x509v3/v3_pcia.o \
	$O\x509v3/v3_ocsp.o \
	$O\x509v3/v3_skey.o \
	$O\x509v3/v3_purp.o \
	$O\x509v3/v3_sxnet.o \
	$O\x509v3/v3_pku.o \
	$O\x509v3/v3_lib.o \
	$O\x509v3/v3_crld.o \
	$O\x509v3/v3_enum.o \
	$O\x509v3/v3_alt.o \
	$O\x509v3/v3_bcons.o \
	$O\x509v3/pcy_data.o \
	$O\x509v3/pcy_cache.o \
	$O\x509v3/v3_int.o \
	$O\x509v3/v3_genn.o \
	$O\x509v3/v3_akeya.o \
	$O\x509v3/v3_bitst.o \
	$O\x509v3/v3_utl.o \
	$O\x509v3/v3_prn.o \
	$O\x509v3/pcy_tree.o \
	$O\x509v3/v3_ncons.o \
	$O\x509v3/v3_info.o \
	$O\x509v3/v3_cpols.o \
	$O\x509v3/v3err.o \
	$O\x509v3/v3_extku.o \
	$O\x509v3/v3_akey.o \
	$O\x509v3/pcy_node.o \
	$O\x509v3/v3_conf.o \
	$O\x509v3/pcy_lib.o \
	$O\x509v3/v3_pmaps.o \
	$O\bf/bf_skey.o \
	$O\bf/bf_enc.o \
	$O\bf/bf_ecb.o \
	$O\bf/bf_cfb64.o \
	$O\bf/bf_ofb64.o \
	$O\gost/gostr341001_ameth.o \
	$O\gost/gostr341001_pmeth.o \
	$O\gost/gost_err.o \
	$O\gost/gostr341001.o \
	$O\gost/gost_asn1.o \
	$O\gost/gost89imit_ameth.o \
	$O\gost/gostr341194.o \
	$O\gost/gostr341001_key.o \
	$O\gost/gost89imit_pmeth.o \
	$O\gost/gost89_params.o \
	$O\gost/streebog.o \
	$O\gost/gost2814789.o \
	$O\gost/gost89_keywrap.o \
	$O\gost/gostr341001_params.o \
	$O\idea/i_cfb64.o \
	$O\idea/i_skey.o \
	$O\idea/i_cbc.o \
	$O\idea/i_ofb64.o \
	$O\idea/i_ecb.o \
	$O\buffer/buffer.o \
	$O\buffer/buf_str.o \
	$O\buffer/buf_err.o \
	$O\o_time.o \
	$O\cpt_err.o \
	$O\cryptlib.o \
	$O\modes/xts128.o \
	$O\modes/ccm128.o \
	$O\modes/gcm128.o \
	$O\modes/ofb128.o \
	$O\modes/cts128.o \
	$O\modes/cbc128.o \
	$O\modes/ctr128.o \
	$O\modes/cfb128.o \
	$O\rra_portable/inet_aton.o \
	$O\rra_portable/getnameinfo.o \
	$O\rra_portable/snprintf.o \
	$O\rra_portable/inet_ntoa.o \
	$O\rra_portable/getaddrinfo.o \
	$O\rra_portable/inet_ntop.o \
	$O\rra_portable/inet_pton.o \
	$O\md4/md4_dgst.o \
	$O\md4/md4_one.o \
	$O\dso/dso_lib.o \
	$O\dso/dso_dlfcn.o \
	$O\dso/dso_openssl.o \
	$O\dso/dso_null.o \
	$O\dso/dso_err.o \
	$O\hkdf/hkdf.o \
	$O\stack/stack.o \
	$O\txt_db/txt_db.o \
	$O\bn/bn_mpi.o \
	$O\bn/bn_asm.o \
	$O\bn/bn_prime.o \
	$O\bn/bn_const.o \
	$O\bn/bn_depr.o \
	$O\bn/bn_nist.o \
	$O\bn/bn_gf2m.o \
	$O\bn/bn_sqr.o \
	$O\bn/bn_rand.o \
	$O\bn/bn_err.o \
	$O\bn/bn_ctx.o \
	$O\bn/bn_lib.o \
	$O\bn/bn_mont.o \
	$O\bn/bn_mod.o \
	$O\bn/bn_mul.o \
	$O\bn/bn_recp.o \
	$O\bn/bn_kron.o \
	$O\bn/bn_gcd.o \
	$O\bn/bn_exp2.o \
	$O\bn/bn_x931p.o \
	$O\bn/bn_div.o \
	$O\bn/bn_shift.o \
	$O\bn/bn_add.o \
	$O\bn/bn_word.o \
	$O\bn/bn_exp.o \
	$O\bn/bn_sqrt.o \
	$O\bn/bn_print.o \
	$O\bn/bn_blind.o \
	$O\dh/dh_depr.o \
	$O\dh/dh_key.o \
	$O\dh/dh_check.o \
	$O\dh/dh_pmeth.o \
	$O\dh/dh_gen.o \
	$O\dh/dh_prn.o \
	$O\dh/dh_asn1.o \
	$O\dh/dh_err.o \
	$O\dh/dh_ameth.o \
	$O\dh/dh_lib.o \
	$O\ripemd/rmd_one.o \
	$O\ripemd/rmd_dgst.o \
	$O\conf/conf_sap.o \
	$O\conf/conf_def.o \
	$O\conf/conf_lib.o \
	$O\conf/conf_mall.o \
	$O\conf/conf_mod.o \
	$O\conf/conf_err.o \
	$O\conf/conf_api.o \
	$O\ecdh/ech_lib.o \
	$O\ecdh/ech_err.o \
	$O\ecdh/ech_key.o \
	$O\mem_clr.o \
	$O\err/err_prn.o \
	$O\err/err.o \
	$O\err/err_all.o \
	$O\pkcs7/pk7_attr.o \
	$O\pkcs7/pkcs7err.o \
	$O\pkcs7/pk7_asn1.o \
	$O\pkcs7/pk7_doit.o \
	$O\pkcs7/pk7_smime.o \
	$O\pkcs7/pk7_mime.o \
	$O\pkcs7/bio_pk7.o \
	$O\pkcs7/pk7_lib.o \
	$O\comp/comp_err.o \
	$O\comp/comp_lib.o \
	$O\comp/c_rle.o \
	$O\comp/c_zlib.o \
	$O\lhash/lh_stats.o \
	$O\lhash/lhash.o \
	$O\malloc-wrapper.o \
	$O\hmac/hmac.o \
	$O\hmac/hm_ameth.o \
	$O\hmac/hm_pmeth.o \
	$O\rand/rand_lib.o \
	$O\rand/randfile.o \
	$O\rand/rand_err.o \
	$O\ocsp/ocsp_prn.o \
	$O\ocsp/ocsp_err.o \
	$O\ocsp/ocsp_ext.o \
	$O\ocsp/ocsp_vfy.o \
	$O\ocsp/ocsp_ht.o \
	$O\ocsp/ocsp_cl.o \
	$O\ocsp/ocsp_lib.o \
	$O\ocsp/ocsp_srv.o \
	$O\ocsp/ocsp_asn.o \
	$O\x509/x509spki.o \
	$O\x509/x509_v3.o \
	$O\x509/by_dir.o \
	$O\x509/x509_r2x.o \
	$O\x509/x509rset.o \
	$O\x509/x509_lu.o \
	$O\x509/x509_err.o \
	$O\x509/x509_d2.o \
	$O\x509/x509_def.o \
	$O\x509/x509_vfy.o \
	$O\x509/x_all.o \
	$O\x509/by_mem.o \
	$O\x509/x509_att.o \
	$O\x509/x509_ext.o \
	$O\x509/x509_set.o \
	$O\x509/x509_vpm.o \
	$O\x509/x509_txt.o \
	$O\x509/x509name.o \
	$O\x509/by_file.o \
	$O\x509/x509_req.o \
	$O\x509/x509type.o \
	$O\x509/x509cset.o \
	$O\x509/x509_obj.o \
	$O\x509/x509_trs.o \
	$O\x509/x509_cmp.o \
	$O\ui/ui_err.o \
	$O\ui/ui_util.o \
	$O\ui/ui_lib.o \
	$O\crypto_init.o \
	$O\o_init.o \
	$O\cversion.o \
	$O\poly1305/poly1305.o \
	$O\ex_data.o \
	$O\rc4/rc4_skey.o \
	$O\rc4/rc4_enc.o \
	$O\bio/bss_dgram.o \
	$O\bio/bss_mem.o \
	$O\bio/b_print.o \
	$O\bio/bss_null.o \
	$O\bio/bio_cb.o \
	$O\bio/bio_lib.o \
	$O\bio/bio_err.o \
	$O\bio/b_dump.o \
	$O\bio/bss_conn.o \
	$O\bio/bf_buff.o \
	$O\bio/bf_null.o \
	$O\bio/bf_nbio.o \
	$O\bio/bss_acpt.o \
	$O\bio/b_sock.o \
	$O\bio/bss_sock.o \
	$O\bio/bss_file.o \
	$O\bio/bss_bio.o \
	$O\bio/bss_fd.o \
	$O\bio/bio_meth.o \
	$O\sha/sha256.o \
	$O\sha/sha1dgst.o \
	$O\sha/sha512.o \
	$O\sha/sha1_one.o \
	$O\des/set_key.o \
	$O\des/pcbc_enc.o \
	$O\des/ofb_enc.o \
	$O\des/fcrypt.o \
	$O\des/cbc_cksm.o \
	$O\des/qud_cksm.o \
	$O\des/ecb_enc.o \
	$O\des/fcrypt_b.o \
	$O\des/rand_key.o \
	$O\des/ofb64ede.o \
	$O\des/enc_read.o \
	$O\des/cbc_enc.o \
	$O\des/xcbc_enc.o \
	$O\des/ecb3_enc.o \
	$O\des/enc_writ.o \
	$O\des/cfb64ede.o \
	$O\des/des_enc.o \
	$O\des/ede_cbcm_enc.o \
	$O\des/cfb_enc.o \
	$O\des/ofb64enc.o \
	$O\des/str2key.o \
	$O\des/cfb64enc.o \
	$O\camellia/cmll_ofb.o \
	$O\camellia/cmll_ctr.o \
	$O\camellia/cmll_cbc.o \
	$O\camellia/camellia.o \
	$O\camellia/cmll_ecb.o \
	$O\camellia/cmll_misc.o \
	$O\camellia/cmll_cfb.o \
	$O\rsa/rsa_lib.o \
	$O\rsa/rsa_chk.o \
	$O\rsa/rsa_depr.o \
	$O\rsa/rsa_eay.o \
	$O\rsa/rsa_crpt.o \
	$O\rsa/rsa_err.o \
	$O\rsa/rsa_oaep.o \
	$O\rsa/rsa_x931.o \
	$O\rsa/rsa_pss.o \
	$O\rsa/rsa_saos.o \
	$O\rsa/rsa_pk1.o \
	$O\rsa/rsa_meth.o \
	$O\rsa/rsa_asn1.o \
	$O\rsa/rsa_gen.o \
	$O\rsa/rsa_sign.o \
	$O\rsa/rsa_prn.o \
	$O\rsa/rsa_ameth.o \
	$O\rsa/rsa_pmeth.o \
	$O\rsa/rsa_none.o \
	$O\compat/freezero.o \
	$O\compat/strndup.o \
	$O\compat/bsd-asprintf.o \
	$O\compat/strlcpy.o \
	$O\compat/timegm.o \
	$O\compat/strnlen.o \
	$O\compat/recallocarray.o \
	$O\compat/reallocarray.o \
	$O\compat/timingsafe_bcmp.o \
	$O\compat/strsep.o \
	$O\compat/timingsafe_memcmp.o \
	$O\compat/strlcat.o \
	$O\cmac/cm_pmeth.o \
	$O\cmac/cmac.o \
	$O\cmac/cm_ameth.o \
	$O\asn1/x_pubkey.o \
	$O\asn1/a_time_tm.o \
	$O\asn1/asn1_gen.o \
	$O\asn1/evp_asn1.o \
	$O\asn1/bio_ndef.o \
	$O\asn1/t_crl.o \
	$O\asn1/tasn_prn.o \
	$O\asn1/x_x509a.o \
	$O\asn1/x_sig.o \
	$O\asn1/asn1_lib.o \
	$O\asn1/a_mbstr.o \
	$O\asn1/t_spki.o \
	$O\asn1/a_digest.o \
	$O\asn1/asn_moid.o \
	$O\asn1/bio_asn1.o \
	$O\asn1/a_strnid.o \
	$O\asn1/i2d_pr.o \
	$O\asn1/ameth_lib.o \
	$O\asn1/tasn_typ.o \
	$O\asn1/nsseq.o \
	$O\asn1/a_i2d_fp.o \
	$O\asn1/t_pkey.o \
	$O\asn1/x_attrib.o \
	$O\asn1/a_set.o \
	$O\asn1/tasn_fre.o \
	$O\asn1/x_bignum.o \
	$O\asn1/x_info.o \
	$O\asn1/a_dup.o \
	$O\asn1/a_d2i_fp.o \
	$O\asn1/a_verify.o \
	$O\asn1/p8_pkey.o \
	$O\asn1/tasn_utl.o \
	$O\asn1/a_sign.o \
	$O\asn1/a_object.o \
	$O\asn1/a_bytes.o \
	$O\asn1/x_nx509.o \
	$O\asn1/x_exten.o \
	$O\asn1/x_val.o \
	$O\asn1/t_req.o \
	$O\asn1/n_pkey.o \
	$O\asn1/x_long.o \
	$O\asn1/a_print.o \
	$O\asn1/a_time.o \
	$O\asn1/d2i_pu.o \
	$O\asn1/asn1_par.o \
	$O\asn1/x_spki.o \
	$O\asn1/x_pkey.o \
	$O\asn1/a_enum.o \
	$O\asn1/x_req.o \
	$O\asn1/i2d_pu.o \
	$O\asn1/a_octet.o \
	$O\asn1/tasn_dec.o \
	$O\asn1/tasn_enc.o \
	$O\asn1/t_x509a.o \
	$O\asn1/tasn_new.o \
	$O\asn1/p5_pbev2.o \
	$O\asn1/a_strex.o \
	$O\asn1/t_x509.o \
	$O\asn1/x_crl.o \
	$O\asn1/asn_mime.o \
	$O\asn1/a_bitstr.o \
	$O\asn1/asn1_err.o \
	$O\asn1/x_x509.o \
	$O\asn1/a_type.o \
	$O\asn1/x_algor.o \
	$O\asn1/d2i_pr.o \
	$O\asn1/p5_pbe.o \
	$O\asn1/a_int.o \
	$O\asn1/asn_pack.o \
	$O\asn1/x_name.o \
	$O\asn1/a_utf8.o \
	$O\asn1/a_bool.o \
	$O\asn1/f_int.o \
	$O\asn1/f_enum.o \
	$O\asn1/f_string.o \
	$O\asn1/t_bitst.o \
	$O\mem_dbg.o \
	$O\curve25519/curve25519.o \
	$O\curve25519/curve25519-generic.o \
	$O\md5/md5_one.o \
	$O\md5/md5_dgst.o \
	$O\pem/pem_pkey.o \
	$O\pem/pem_lib.o \
	$O\pem/pem_xaux.o \
	$O\pem/pem_pk8.o \
	$O\pem/pem_info.o \
	$O\pem/pem_seal.o \
	$O\pem/pem_all.o \
	$O\pem/pem_sign.o \
	$O\pem/pem_oth.o \
	$O\pem/pvkfmt.o \
	$O\pem/pem_err.o \
	$O\pem/pem_x509.o \
	$O\evp/e_aes.o \
	$O\evp/m_ripemd.o \
	$O\evp/e_chacha.o \
	$O\evp/p_enc.o \
	$O\evp/m_md5.o \
	$O\evp/pmeth_gn.o \
	$O\evp/bio_md.o \
	$O\evp/p_lib.o \
	$O\evp/evp_pkey.o \
	$O\evp/e_rc2.o \
	$O\evp/p_open.o \
	$O\evp/m_ecdsa.o \
	$O\evp/e_idea.o \
	$O\evp/e_bf.o \
	$O\evp/p_dec.o \
	$O\evp/evp_err.o \
	$O\evp/m_sigver.o \
	$O\evp/p_seal.o \
	$O\evp/m_dss1.o \
	$O\evp/bio_b64.o \
	$O\evp/p5_crpt2.o \
	$O\evp/bio_enc.o \
	$O\evp/p_sign.o \
	$O\evp/e_des.o \
	$O\evp/e_cast.o \
	$O\evp/pmeth_lib.o \
	$O\evp/m_dss.o \
	$O\evp/m_md5_sha1.o \
	$O\evp/e_chacha20poly1305.o \
	$O\evp/evp_pbe.o \
	$O\evp/encode.o \
	$O\evp/m_md4.o \
	$O\evp/c_all.o \
	$O\evp/names.o \
	$O\evp/p5_crpt.o \
	$O\evp/m_gostr341194.o \
	$O\evp/evp_aead.o \
	$O\evp/e_old.o \
	$O\evp/evp_key.o \
	$O\evp/evp_enc.o \
	$O\evp/e_gost2814789.o \
	$O\evp/e_rc4_hmac_md5.o \
	$O\evp/e_aes_cbc_hmac_sha1.o \
	$O\evp/e_des3.o \
	$O\evp/m_sha1.o \
	$O\evp/e_rc4.o \
	$O\evp/m_null.o \
	$O\evp/e_null.o \
	$O\evp/pmeth_fn.o \
	$O\evp/m_wp.o \
	$O\evp/p_verify.o \
	$O\evp/digest.o \
	$O\evp/m_gost2814789.o \
	$O\evp/m_streebog.o \
	$O\evp/evp_lib.o \
	$O\evp/e_xcbc_d.o \
	$O\evp/e_camellia.o \
	$O\engine/eng_cnf.o \
	$O\engine/tb_dsa.o \
	$O\engine/eng_fat.o \
	$O\engine/tb_digest.o \
	$O\engine/eng_ctrl.o \
	$O\engine/tb_pkmeth.o \
	$O\engine/tb_asnmth.o \
	$O\engine/tb_cipher.o \
	$O\engine/eng_list.o \
	$O\engine/tb_store.o \
	$O\engine/eng_openssl.o \
	$O\engine/eng_all.o \
	$O\engine/eng_err.o \
	$O\engine/eng_lib.o \
	$O\engine/eng_table.o \
	$O\engine/tb_ecdsa.o \
	$O\engine/tb_ecdh.o \
	$O\engine/eng_dyn.o \
	$O\engine/eng_init.o \
	$O\engine/tb_dh.o \
	$O\engine/eng_pkey.o \
	$O\engine/tb_rsa.o \
	$O\engine/tb_rand.o \
	$O\objects/obj_xref.o \
	$O\objects/obj_dat.o \
	$O\objects/obj_err.o \
	$O\objects/obj_lib.o \
	$O\objects/o_names.o \
	$O\o_str.o \
	$O\pkcs12/p12_p8d.o \
	$O\pkcs12/p12_attr.o \
	$O\pkcs12/p12_asn.o \
	$O\pkcs12/p12_mutl.o \
	$O\pkcs12/p12_crt.o \
	$O\pkcs12/p12_init.o \
	$O\pkcs12/p12_npas.o \
	$O\pkcs12/p12_key.o \
	$O\pkcs12/p12_add.o \
	$O\pkcs12/p12_p8e.o \
	$O\pkcs12/p12_utl.o \
	$O\pkcs12/p12_kiss.o \
	$O\pkcs12/p12_crpt.o \
	$O\pkcs12/p12_decr.o \
	$O\pkcs12/pk12err.o \
	$O\aes/aes_ige.o \
	$O\aes/aes_ofb.o \
	$O\aes/aes_cbc.o \
	$O\aes/aes_cfb.o \
	$O\aes/aes_wrap.o \
	$O\aes/aes_ecb.o \
	$O\aes/aes_core.o \
	$O\aes/aes_ctr.o \
	$O\aes/aes_misc.o \
	$O\cast/c_ecb.o \
	$O\cast/c_enc.o \
	$O\cast/c_ofb64.o \
	$O\cast/c_skey.o \
	$O\cast/c_cfb64.o \
	$O\ec/ec_ameth.o \
	$O\ec/ec2_smpl.o \
	$O\ec/ec_oct.o \
	$O\ec/ec_err.o \
	$O\ec/ec_cvt.o \
	$O\ec/ecp_smpl.o \
	$O\ec/ec_lib.o \
	$O\ec/ec_mult.o \
	$O\ec/eck_prn.o \
	$O\ec/ec_check.o \
	$O\ec/ec_key.o \
	$O\ec/ecp_mont.o \
	$O\ec/ec2_oct.o \
	$O\ec/ec_pmeth.o \
	$O\ec/ec2_mult.o \
	$O\ec/ec_curve.o \
	$O\ec/ec_asn1.o \
	$O\ec/ec_print.o \
	$O\ec/ecp_nist.o \
	$O\ec/ecp_oct.o \
	$O\compat/explicit_bzero.o \
	$O\bio/b_posix.o \
	$O\ui/ui_openssl.o \
	$O\bio/bss_log.o \
	$O\dll_main.o \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

PRODUCT_DLIBS= \
	__mkg_sentinel_target__ \
	$(DLIB_FILE0) \

PRODUCT_SLIBS= \
	__mkg_sentinel_target__ \
	$(SLIB_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \



# Entry rule.
all: $O\ts $O\dsa $O\chacha $O\ecdsa $O\rc2 $O\whrlpool $O\x509v3 $O\bf $O\gost $O\idea $O\buffer $O\modes $O\rra_portable $O\md4 $O\dso $O\hkdf $O\stack $O\txt_db $O\bn $O\dh $O\ripemd $O\conf $O\ecdh $O\err $O\pkcs7 $O\comp $O\lhash $O\hmac $O\rand $O\ocsp $O\x509 $O\ui $O\poly1305 $O\rc4 $O\bio $O\sha $O\des $O\camellia $O\rsa $O\compat $O\cmac $O\asn1 $O\curve25519 $O\md5 $O\pem $O\evp $O\engine $O\objects $O\pkcs12 $O\aes $O\cast $O\ec $O $(DLIB_FILE0) $(SLIB_FILE0) 

# Mkdir rule.
$O\ts:
	if not exist $O\ts mkdir $O\ts

$O\dsa:
	if not exist $O\dsa mkdir $O\dsa

$O\chacha:
	if not exist $O\chacha mkdir $O\chacha

$O\ecdsa:
	if not exist $O\ecdsa mkdir $O\ecdsa

$O\rc2:
	if not exist $O\rc2 mkdir $O\rc2

$O\whrlpool:
	if not exist $O\whrlpool mkdir $O\whrlpool

$O\x509v3:
	if not exist $O\x509v3 mkdir $O\x509v3

$O\bf:
	if not exist $O\bf mkdir $O\bf

$O\gost:
	if not exist $O\gost mkdir $O\gost

$O\idea:
	if not exist $O\idea mkdir $O\idea

$O\buffer:
	if not exist $O\buffer mkdir $O\buffer

$O\modes:
	if not exist $O\modes mkdir $O\modes

$O\rra_portable:
	if not exist $O\rra_portable mkdir $O\rra_portable

$O\md4:
	if not exist $O\md4 mkdir $O\md4

$O\dso:
	if not exist $O\dso mkdir $O\dso

$O\hkdf:
	if not exist $O\hkdf mkdir $O\hkdf

$O\stack:
	if not exist $O\stack mkdir $O\stack

$O\txt_db:
	if not exist $O\txt_db mkdir $O\txt_db

$O\bn:
	if not exist $O\bn mkdir $O\bn

$O\dh:
	if not exist $O\dh mkdir $O\dh

$O\ripemd:
	if not exist $O\ripemd mkdir $O\ripemd

$O\conf:
	if not exist $O\conf mkdir $O\conf

$O\ecdh:
	if not exist $O\ecdh mkdir $O\ecdh

$O\err:
	if not exist $O\err mkdir $O\err

$O\pkcs7:
	if not exist $O\pkcs7 mkdir $O\pkcs7

$O\comp:
	if not exist $O\comp mkdir $O\comp

$O\lhash:
	if not exist $O\lhash mkdir $O\lhash

$O\hmac:
	if not exist $O\hmac mkdir $O\hmac

$O\rand:
	if not exist $O\rand mkdir $O\rand

$O\ocsp:
	if not exist $O\ocsp mkdir $O\ocsp

$O\x509:
	if not exist $O\x509 mkdir $O\x509

$O\ui:
	if not exist $O\ui mkdir $O\ui

$O\poly1305:
	if not exist $O\poly1305 mkdir $O\poly1305

$O\rc4:
	if not exist $O\rc4 mkdir $O\rc4

$O\bio:
	if not exist $O\bio mkdir $O\bio

$O\sha:
	if not exist $O\sha mkdir $O\sha

$O\des:
	if not exist $O\des mkdir $O\des

$O\camellia:
	if not exist $O\camellia mkdir $O\camellia

$O\rsa:
	if not exist $O\rsa mkdir $O\rsa

$O\compat:
	if not exist $O\compat mkdir $O\compat

$O\cmac:
	if not exist $O\cmac mkdir $O\cmac

$O\asn1:
	if not exist $O\asn1 mkdir $O\asn1

$O\curve25519:
	if not exist $O\curve25519 mkdir $O\curve25519

$O\md5:
	if not exist $O\md5 mkdir $O\md5

$O\pem:
	if not exist $O\pem mkdir $O\pem

$O\evp:
	if not exist $O\evp mkdir $O\evp

$O\engine:
	if not exist $O\engine mkdir $O\engine

$O\objects:
	if not exist $O\objects mkdir $O\objects

$O\pkcs12:
	if not exist $O\pkcs12 mkdir $O\pkcs12

$O\aes:
	if not exist $O\aes mkdir $O\aes

$O\cast:
	if not exist $O\cast mkdir $O\cast

$O\ec:
	if not exist $O\ec mkdir $O\ec

$O:
	if not exist $O mkdir $O


# Product files rule.
$(SLIB_FILE0): $(OBJS0)
	if exist $(SLIB_FILE0) del $(SLIB_FILE0)
	@echo $(LIBAR) crsD $(SLIB_FILE0) {[objs]} $(SUB_OBJS_ECHO)
	@     $(LIBAR) crsD $(SLIB_FILE0) $(OBJS0) $(SUB_OBJS)

gsl.myf: $(SLIB_FILE0)
	@if exist $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe -g gsl.myf $(SLIB_FILE0) $(MACHINE)

gsl.def: gsl.myf
	@if exist $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe -d gsl.myf gsl.def

$(DLIB_FILE0): $(OBJS0)
	if exist $(DLIB_FILE0) del $(DLIB_FILE0)
	@echo $(LINKER) -shared -Wl,-soname,$(DLIB_NAME0) \
	-lgcc {[objs]} $(SUB_LIBS) -Wl,-rpath,.  $(LINKER_OPT) -o $(DLIB_FILE0)
	@     $(LINKER) -shared -Wl,-soname,$(DLIB_NAME0) \
	-lgcc $(OBJS0) $(SUB_LIBS) -Wl,-rpath,.  $(LINKER_OPT) -o $(DLIB_FILE0)
	$(STRIP_UNNEEDED) $(DLIB_FILE0)


##
# Pattern rule.
# We use not suffix rule but pattern rule for dealing flexibly with files in sub-directory.
# In this case, there is very confusing specification, that is :
# '\' to the left hand of ':' works as escape sequence, 
# '\' to the right hand of ':' does not work as escape sequence. 
# Hence, we have to duplicate '\' to the left hand of ':',
# the other way, '\' to the right hand of ':' we have to put only single '\',
# for example $O\\%.o: $S\%.c .
#
$O\\ts\\%.o: $S\ts\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\ts\\%.o: $S\ts\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\dsa\\%.o: $S\dsa\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\dsa\\%.o: $S\dsa\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\chacha\\%.o: $S\chacha\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\chacha\\%.o: $S\chacha\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\ecdsa\\%.o: $S\ecdsa\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\ecdsa\\%.o: $S\ecdsa\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\rc2\\%.o: $S\rc2\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\rc2\\%.o: $S\rc2\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\whrlpool\\%.o: $S\whrlpool\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\whrlpool\\%.o: $S\whrlpool\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\x509v3\\%.o: $S\x509v3\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\x509v3\\%.o: $S\x509v3\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\bf\\%.o: $S\bf\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\bf\\%.o: $S\bf\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\gost\\%.o: $S\gost\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\gost\\%.o: $S\gost\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\idea\\%.o: $S\idea\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\idea\\%.o: $S\idea\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\buffer\\%.o: $S\buffer\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\buffer\\%.o: $S\buffer\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\modes\\%.o: $S\modes\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\modes\\%.o: $S\modes\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\rra_portable\\%.o: $S\rra_portable\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\rra_portable\\%.o: $S\rra_portable\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\md4\\%.o: $S\md4\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\md4\\%.o: $S\md4\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\dso\\%.o: $S\dso\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\dso\\%.o: $S\dso\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\hkdf\\%.o: $S\hkdf\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\hkdf\\%.o: $S\hkdf\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\stack\\%.o: $S\stack\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\stack\\%.o: $S\stack\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\txt_db\\%.o: $S\txt_db\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\txt_db\\%.o: $S\txt_db\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\bn\\%.o: $S\bn\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\bn\\%.o: $S\bn\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\dh\\%.o: $S\dh\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\dh\\%.o: $S\dh\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\ripemd\\%.o: $S\ripemd\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\ripemd\\%.o: $S\ripemd\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\conf\\%.o: $S\conf\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\conf\\%.o: $S\conf\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\ecdh\\%.o: $S\ecdh\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\ecdh\\%.o: $S\ecdh\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\err\\%.o: $S\err\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\err\\%.o: $S\err\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\pkcs7\\%.o: $S\pkcs7\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\pkcs7\\%.o: $S\pkcs7\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\comp\\%.o: $S\comp\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\comp\\%.o: $S\comp\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\lhash\\%.o: $S\lhash\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\lhash\\%.o: $S\lhash\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\hmac\\%.o: $S\hmac\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\hmac\\%.o: $S\hmac\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\rand\\%.o: $S\rand\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\rand\\%.o: $S\rand\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\ocsp\\%.o: $S\ocsp\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\ocsp\\%.o: $S\ocsp\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\x509\\%.o: $S\x509\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\x509\\%.o: $S\x509\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\ui\\%.o: $S\ui\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\ui\\%.o: $S\ui\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\poly1305\\%.o: $S\poly1305\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\poly1305\\%.o: $S\poly1305\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\rc4\\%.o: $S\rc4\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\rc4\\%.o: $S\rc4\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\bio\\%.o: $S\bio\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\bio\\%.o: $S\bio\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\sha\\%.o: $S\sha\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\sha\\%.o: $S\sha\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\des\\%.o: $S\des\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\des\\%.o: $S\des\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\camellia\\%.o: $S\camellia\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\camellia\\%.o: $S\camellia\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\rsa\\%.o: $S\rsa\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\rsa\\%.o: $S\rsa\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\compat\\%.o: $S\compat\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\compat\\%.o: $S\compat\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\cmac\\%.o: $S\cmac\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\cmac\\%.o: $S\cmac\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\asn1\\%.o: $S\asn1\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\asn1\\%.o: $S\asn1\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\curve25519\\%.o: $S\curve25519\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\curve25519\\%.o: $S\curve25519\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\md5\\%.o: $S\md5\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\md5\\%.o: $S\md5\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\pem\\%.o: $S\pem\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\pem\\%.o: $S\pem\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\evp\\%.o: $S\evp\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\evp\\%.o: $S\evp\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\engine\\%.o: $S\engine\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\engine\\%.o: $S\engine\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\objects\\%.o: $S\objects\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\objects\\%.o: $S\objects\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\pkcs12\\%.o: $S\pkcs12\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\pkcs12\\%.o: $S\pkcs12\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\aes\\%.o: $S\aes\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\aes\\%.o: $S\aes\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\cast\\%.o: $S\cast\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\cast\\%.o: $S\cast\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\ec\\%.o: $S\ec\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\ec\\%.o: $S\ec\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\%.o: $S\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\%.o: $S\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<


# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:

# Install exec rule.
install_exec:
	@for %%a in ( $(RUNTIME_FILES) ) do @if exist "%%a" @$(CP) /F "%%a" $(MY_LIBS_ROOT)\$(DLIBS_DIR)\ $(CP_END)

# Install dlib rule.
install_dlib: $(DLIB_FILE0)
	@if not exist $(MY_LIBS_ROOT)\$(DLIBS_DIR) @mkdir $(MY_LIBS_ROOT)\$(DLIBS_DIR) 
	@if exist "$(DLIB_FILE0)" @$(CP) /F "$(DLIB_FILE0)" $(MY_LIBS_ROOT)\$(DLIBS_DIR)\ $(CP_END)

# Install slib rule.
install_slib: $(SLIB_FILE0)
	@if not exist $(MY_LIBS_ROOT)\$(SLIBS_DIR) @mkdir $(MY_LIBS_ROOT)\$(SLIBS_DIR) 
	@if exist "$(SLIB_FILE0)" @$(CP) /F "$(SLIB_FILE0)" $(MY_LIBS_ROOT)\$(SLIBS_DIR)\ $(CP_END)

# Install rule.
install: all install_slib install_dlib 


# Clean rule.
clean:
	rmdir /S /Q $O\ 

# Src and Headers Dependency
whrlpool/wp_block.o: x86_arch.h
gost/gostr341194.o: md32_common.h
gost/gost2814789.o: md32_common.h
o_time.o: o_time.h
modes/gcm128.o: x86_arch.h
rra_portable/inet_aton.o: rra_portable/inet_conv.h
rra_portable/getnameinfo.o: rra_portable/getnameinfo.h rra_portable/socket.h rra_portable/snprintf.h
rra_portable/snprintf.o: rra_portable/snprintf.h
rra_portable/inet_ntoa.o: rra_portable/inet_conv.h rra_portable/snprintf.h
rra_portable/getaddrinfo.o: rra_portable/getaddrinfo.h rra_portable/inet_conv.h
rra_portable/inet_ntop.o: rra_portable/inet_conv.h rra_portable/snprintf.h rra_portable/socket.h
bn/bn_exp.o: constant_time_locl.h
x509/x509_def.o: cryptlib.h
crypto_init.o: cryptlib.h
cversion.o: cryptlib.h
bio/b_sock.o: rra_portable/getaddrinfo.h rra_portable/getnameinfo.h
sha/sha256.o: md32_common.h
asn1/a_time_tm.o: o_time.h
asn1/a_time.o: o_time.h
evp/e_aes.o: x86_arch.h
evp/c_all.o: cryptlib.h
evp/e_rc4_hmac_md5.o: x86_arch.h
evp/e_aes_cbc_hmac_sha1.o: constant_time_locl.h x86_arch.h
engine/eng_list.o: cryptlib.h
engine/eng_all.o: cryptlib.h
