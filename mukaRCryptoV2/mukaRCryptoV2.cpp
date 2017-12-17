#include "IceKey.H"

using namespace System;
using namespace System::Text;
using namespace System::IO;
using namespace System::Security::Cryptography;

int main()
{
	Console::WriteLine(L"輸入路徑 :");
	String^ path = Console::ReadLine();
	Console::WriteLine(L"1. 加密 2. 解密");
	auto readkey = Console::ReadKey(true);
	auto ice = new IceKey(0);
	auto dir = gcnew DirectoryInfo(path);
	array<FileInfo ^> ^ files;
	bool flag = false;
	if (readkey.KeyChar == L'1') {
		files = dir->GetFiles(L"*.png", SearchOption::AllDirectories);
		flag = true;
	}
	else if (readkey.KeyChar == L'2') {
		files = dir->GetFiles(L"*.mukaR",SearchOption::AllDirectories);
	}
	else {
		return 0;
	}
	for each (auto file in files)
	{
		if (flag)
			Console::WriteLine(L"加密：" + file->Name);
		else
			Console::WriteLine(L"解密：" + file->Name);
		auto filename = file->Name->Replace(file->Extension, L"");
		auto outputpath = file->FullName->Replace(file->Extension, L"");
		auto input = File::ReadAllBytes(file->FullName);
		auto length = input->Length;
		pin_ptr<unsigned char> inputp = &input[0];
		auto output = gcnew array<unsigned char>(length);
		pin_ptr<unsigned char> outputp = &output[0];
		auto md5input = Encoding::UTF8->GetBytes(L"Zhanjian" + filename);
		auto md5 = gcnew MD5CryptoServiceProvider();
		auto md5output = md5->ComputeHash(md5input);
		auto keytext = BitConverter::ToString(md5output)->Replace("-", "")->ToLower();
		auto key = Encoding::UTF8->GetBytes(keytext);
		pin_ptr<unsigned char> keyp = &key[0];
		ice->set(keyp);
		
		if (flag) {
			for (int i = 0; i < length; i = i + 8) {
				ice->encrypt(&inputp[i], &outputp[i]);
			}
			//判斷是否已存在相同檔名，避免覆蓋
			if(File::Exists(outputpath + L".mukaR")) {
				outputpath += L"_1";
			}
			File::WriteAllBytes(outputpath + L".mukaR", output);
		}
		else {
			for (int i = 0; i < length; i = i + 8) {
				ice->decrypt(&inputp[i], &outputp[i]);
			}
			//判斷是否已存在相同檔名，避免覆蓋
			while (File::Exists(outputpath + L".png")) outputpath += L"_1";
			if (File::Exists(outputpath + L".png")) {
				//紀錄重複項，方便加密回mukaR時改名
				StreamWriter^ sw = gcnew StreamWriter("ChangeName.txt");
				sw->WriteLine(outputpath + L".png");
				sw->Close();
			}
			File::WriteAllBytes(outputpath + L".png", output);
		}
	}
	Console::WriteLine(L"所有操作已完成！");
	Console::Read();
	return 0;
}