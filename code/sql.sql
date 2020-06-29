create table student (
		sno char (8), 
		sname char(16) unique, 
		sage int, 
		sgender char (1), 
		primary key ( sno ) 
);
create index stunameidx 
on student ( sname );




insert into student 
values('111', 'A', 11, 'F'),
      ('222', 'B', 22, 'M'),
      ('333', 'C', 33, 'F'),
      ('444', 'D', 44, 'M'),
      ('555', 'E', 55, 'F'),
      ('666', 'F', 66, 'M'),
      ('777', 'G', 77, 'F');


select sno, sname, sage, sgender 
from student 
where sno <> '111';

delete from student 
where sage >= 33 and sage <> 66 and sgender = 'F';

select * 
from student;

delete from student 
where sno = '444';

select sno 
from student;

delete from student;

select * 
from student;


drop index stunameidx on student;
drop table student;

quit;