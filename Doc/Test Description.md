# Build

源码文件夹的[README.txt](../README.txt)中提到了两种编译方式，为了使效果更好，我在测试时使用了CMake。

# Test

## 功能测试

分成两个测试文件进行，一个测试文件是进行常见语句的测试，另一个是测试退出程序后再入是否还能读到表格内容以及进行字符集测试(测试时需要根据不同系统的字体位宽修改MiniSQL.h中的常量`UTF8_WIDTH`，macOS为1.5，Windows为2)。详见视频。

## 性能测试

编写py脚本产生10w组的数据，其中包括字符串和整型，进行插入，选择，删除的操作测试。详见视频。

# 附录

测试脚本：

```bash
#!/bin/zsh
rm -f ./.DBFILE/*

# Build
mkdir build
cd ./build/
cmake ../Src/
make
cd ..

# Test
./MiniSQL < ./Test/input0.txt > ./Test/output0.txt
./MiniSQL < ./Test/input1.txt > ./Test/output1.txt

# Big-Data Test
python3 ./Test/Generator.py
```

测试文件1

```sql
create table People(Name char(256), ID int unique, Age int, Score float, primary key(Name));
create table People(Name char(20), ID int unique, Age int, Score float, primary key(Name));

INSERT into People values('AaA', 0, 19, 11.2);
insert into People VALUES('BbB', 1, 20, 33.4);
insert into People values('CcC', 2, 20, 55.6);
insert into People values('DdD', 3, 98, 66.7);
insert into People values('EeE', 4, 99, 77.8);

INSERT into People values('AaA', 99, 19, 11.2);
insert into People VALUES('XXX', 1, 20, 33.4);

create index index_Name on People(Name);
create index index_ID on People(ID);
create index index_Age on People(Age);

select * from People;
SELECT Name, ID FROM People WHERE Age = 20 AND Name <> 'BbB';
select Name from People where ID > 3 or Age <= 30;

delete from People where ID >= 2 and ID < 4;

select * from People;

drop index index_Name;

quit;
```

测试文件2

```sql
select * from People;

drop table People;

execfile ./Test/exec.txt;

select * from 身份;

quit;
```

execfile

```sql
create table 身份(名字 char(20) unique, 年龄 int);

insert into 身份 values('中文', 0);

insert into 身份 values('🐂🍺', 1);
```
