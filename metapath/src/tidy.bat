@clang-tidy %1 -- --target=x86_64-pc-windows-msvc -std=c17 -O2 -D_WIN64 -DNOMINMAX -DNDEBUG -DWIN32_LEAN_AND_MEAN -DSTRICT_TYPED_ITEMIDS -DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0600 -DWINVER=0x0600 -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -Wall -Wextra -Wshadow -Wimplicit-fallthrough -Wformat=2 -Wundef -Wcomma 1>tidy.log
@rem @clang-tidy %1 -- --target=x86_64-pc-windows-msvc -march=x86-64-v3 -std=c17 -O2 -D_WIN64 -DNOMINMAX -DNDEBUG -DWIN32_LEAN_AND_MEAN -DSTRICT_TYPED_ITEMIDS -DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0601 -DWINVER=0x0601 -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -Wall -Wextra -Wshadow -Wimplicit-fallthrough -Wformat=2 -Wundef -Wcomma 1>tidy.log
