clear

# ./uncamel --extract orig/qquickgeomapgesturearea_p.h orig/qquickgeomapgesturearea.cpp > identifiers.txt

cp --backup=numbered g.h _g.h
cp --backup=numbered g.cpp _g.cpp

./uncamel --replace identifiers.txt g.h g.cpp

diff -Naur a.h g.h > h.diff
diff -Naur a.cpp g.cpp > c.diff
