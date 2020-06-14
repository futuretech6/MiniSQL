import random as r
import faker
f = faker.Faker()

fp = open("./Gen_out.txt", "w")

fp.write("create table People(Name char(20), ID int, Age int);\n");
for i in range(0, 10000):
    fp.write("insert into People values('" + f.name()[:20] + "' ," + str(r.randint(0, 0x7FFFFFFF)) + " ," + str(r.randint(0, 100)) + ");\n",)

# fp.write("create table People(ID int, Age int, primary key(ID));\n");
# for i in range(0, 10000):
#     fp.write("insert into People values(" + str(r.randint(0, 0x7FFFFFFF)) + " ," + str(r.randint(0, 100)) + ");\n",)



fp.close()