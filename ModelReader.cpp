#include "PmxConverter.h"

int main()
{
    system("chcp 65001");

    Assimp::Exporter exp;
    const aiExportFormatDesc *formats;
    size_t count = exp.GetExportFormatCount();
    
    // 输出格式信息
    using namespace std;
    {
        for (size_t i = 0; i < count; i++)
        {
            formats = exp.GetExportFormatDescription(i);
            cout << "ID: " << formats->id
                 << ", Desc: " << formats->description
                 << ", Ext: " << formats->fileExtension << endl;
        }
    }

    using namespace std;
    string inputPath;
    cin>>inputPath;
    read(inputPath);
}