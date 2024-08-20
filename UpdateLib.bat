

//// xcopy /옵션		.복사할파일이 담겨있는 경로		.복사 할 경로 

xcopy /y/s .\Engine\Public\*.*					.\Reference\Inc\
xcopy /y/s .\Engine\bin\*.lib						.\Reference\bin\
xcopy /y/s .\Engine\bin\*.dll						.\Client\bin\
xcopy /y/s .\Engine\bin\ShaderFiles\*.hlsl		.\Client\bin\ShaderFiles\