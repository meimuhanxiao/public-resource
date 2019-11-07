CC=gcc
AFLAGS=-Wall -g
#LDFLAGS= -L ~/work/sqlite/sqlite-arm/lib  -lsqlite3 
LDFLAGS= -lsqlite3 
#OBJS= login.cgi com_set.cgi net_set.cgi net_check.cgi time_set.cgi\
	  time_check.cgi upload.cgi update_system.cgi realdata.cgi

OBJS= realdata.cgi db_query.cgi net_dbdownload.cgi update_stm.cgi\
	  phonenum_show.cgi phonenum_set.cgi dev_set.cgi dev_query.cgi\
		dev_func.cgi dev_func_show.cgi net_check.cgi net_set.cgi\
		time_show.cgi com_set.cgi com_show.cgi dev_sendto_st.cgi
		
run:$(OBJS)

$(OBJS):%.cgi:%.c
	$(CC) $(AFLAGS) $< cgic.c crc.c -o $@ $(LDFLAGS)
	mv *.cgi ./out -f
	 cp out/*.cgi ~/www/cgi-bin/ -f
.PHONY:clean
clean:
	rm ./out/*.cgi

