1 RSAKeyGen.exe
  RSAKeyGen.exe ��ǩ����Կ���ɹ��ߣ�˫�����оͿ����ˡ�

����˵����
	password  ������8 λASCII��,�Ժ�VLRSign.exe�����ݶ�ȡǩ����Կʱ��ȥУ������Ƿ���ȷ��
	Product Name �����Ʒ���ƣ�������49���ַ����Ժ�VLRSign.exe��������Ĳ�Ʒ���Ƽ�����Ӧ��ǩ����Կ��



2 VLRSign.exe

VLRSign.exe ��ǩ������ֱ���8910ƽ̨,��Ҫ�������д��������С�

ѡ��˵����
	-pw 	��ʹ��RSAKeyGen.exe ����ǩ����Կʱ��������룬�������Ƕ�ȡǩ����Կ�Ŀ��
	-pn 	��RSAKeyGen.exe ����ǩ����Կʱ����Ĳ�Ʒ���ƣ����ڼ���ǩ����Կ��
	-plen 	�����Ҫǩ���Ķ������ĳ��ȡ����8910ƽ̨ nor_fdl1.img nor_bootloader.img -plen �̶�Ϊ0xbce0��
	-ha   	��ǩ����ʹ�ú���hash�㷨�����8910ƽ̨ nor_fdl1.img nor_bootloader.img -ha �̶�ʹ��Blake2��
		nor_fdl.bin �Լ� ϵͳ�ļ������磺UIX8910_UIS8910C_128X128_320X240_refphone_stone_MX.bin��ʹ�� SHA1-32
	-img  	ָ��ǩ�����ļ�·�����ļ�����
	-out  	�ƶ�ǩ����ɺ������ļ�·�����ļ�����
	-pw2	�˰��и� ��-pw�� һ�� ���ڶ� nor_fdl.binǩ��ʱ��Ҫ��
	-pn2	�˰��и� ��-pn��һ�½��ڶ� nor_fdl.binǩ��ʱ��Ҫ��
	-ipbk   �˲������ڶ� nor_fdl.binǩ��ʱ��Ҫ��ָ��Ϊtrue��
	
���ӣ�

.\VLRSign.exe  -pw 12345678 -pn test -plen 0xbce0 -ha Blake2 -img ..\test\data\nor_bootloader.img -out ..\test\data\nor_bootloader_signed.img
.\VLRSign.exe  -pw 12345678 -pn test -plen 0xbce0 -ha Blake2 -img ..\test\data\nor_fdl1.img -out ..\test\data\nor_fdl1_signed.img
.\VLRSign.exe -pw 12345678 -pn test  -ha SHA1-32 -ipbk true -pw2 12345678 -pn2 test -img  ..\test\data\nor_fdl.bin -out ..\test\data\nor_fdl_signed.bin
.\VLRSign.exe  -pw 12345678 -pn test  -ha SHA1-32 -slen 0x100000 -img ..\test\data\UIX8910_UIS8910C_128X128_320X240_refphone_stone_MX.bin -out ..\test\data\UIX8910_UIS8910C_128X128_320X240_refphone_stone_MX_signed.bin

3 8910DM ƽ̨

.\VLRSign.exe  -pw 12345678 -pn test  -ha Blake2 -img ..\test\data\bbapp\fdl1.img -out ..\test\data\bbapp\fdl1_signed.img
.\VLRSign.exe  -pw 12345678 -pn test  -ha Blake2 -img ..\test\data\bbapp\fdl2.img -out ..\test\data\bbapp\fdl2_signed.img
.\VLRSign.exe  -pw 12345678 -pn test  -ha Blake2 -plen 0xbce0 -img ..\test\data\bbapp\boot.img -out ..\test\data\bbapp\boot_signed.img
.\VLRSign.exe  -pw 12345678 -pn test  -ha Blake2 -img ..\test\data\bbapp\8915DM.img -out ..\test\data\bbapp\8915DM_signed.img
