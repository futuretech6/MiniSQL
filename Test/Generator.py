import random as r
import faker
f = faker.Faker()

fp = open("./Test/Gen_out.txt", "w")

fp.write("create table Big(Name char(20), ID int, Age int);\n")
for i in range(0, 100000):
    fp.write("insert into Big values('" + f.name()[:20] + "' ," + str(
        r.randint(0, 0x7FFFFFFF)) + " ," + str(r.randint(0, 100)) + ");\n",)

fp.close()

# ID < 10000000 and Age > 90;