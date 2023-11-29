all:
	@$(MAKE) -C Client
	@$(MAKE) -C Ver_1
	@$(MAKE) -C Ver_2
	@$(MAKE) -C Ver_3
	@$(MAKE) -C Ver_4/async_server
	@$(MAKE) -C Ver_4/async_client

clean:
	@$(MAKE) -C Client clean
	@$(MAKE) -C Ver_1 clean
	@$(MAKE) -C Ver_2 clean
	@$(MAKE) -C Ver_3 clean
	@$(MAKE) -C Ver_4/async_server clean
	@$(MAKE) -C Ver_4/async_client clean

    

