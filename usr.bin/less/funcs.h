	public char * save ();
	public VOID_POINTER ecalloc ();
	public char * skipsp ();
	public int sprefix ();
	public void quit ();
	public void raw_mode ();
	public void scrsize ();
	public char * special_key_str ();
	public void get_term ();
	public void init ();
	public void deinit ();
	public void home ();
	public void add_line ();
	public void remove_top ();
	public void win32_scroll_up ();
	public void lower_left ();
	public void check_winch ();
	public void goto_line ();
	public void vbell ();
	public void bell ();
	public void clear ();
	public void clear_eol ();
	public void clear_bot ();
	public void so_enter ();
	public void so_exit ();
	public void ul_enter ();
	public void ul_exit ();
	public void bo_enter ();
	public void bo_exit ();
	public void bl_enter ();
	public void bl_exit ();
	public void backspace ();
	public void putbs ();
	public char WIN32getch ();
	public void match_brac ();
	public void ch_ungetchar ();
	public void end_logfile ();
	public void sync_logfile ();
	public int ch_seek ();
	public int ch_end_seek ();
	public int ch_beg_seek ();
	public POSITION ch_length ();
	public POSITION ch_tell ();
	public int ch_forw_get ();
	public int ch_back_get ();
	public void ch_setbufspace ();
	public void ch_flush ();
	public int seekable ();
	public void ch_init ();
	public void ch_close ();
	public int ch_getflags ();
	public void ch_dump ();
	public void init_charset ();
	public int binary_char ();
	public int control_char ();
	public char * prchar ();
	public void cmd_reset ();
	public void clear_cmd ();
	public void cmd_putstr ();
	public int len_cmdbuf ();
	public void set_mlist ();
	public void cmd_addhist ();
	public void cmd_accept ();
	public int cmd_char ();
	public LINENUM cmd_int ();
	public char * get_cmdbuf ();
	public int in_mca ();
	public void dispversion ();
	public int getcc ();
	public void ungetcc ();
	public void ungetsc ();
	public void commands ();
	public void init_cmds ();
	public void add_fcmd_table ();
	public void add_ecmd_table ();
	public int fcmd_decode ();
	public int ecmd_decode ();
	public char * lgetenv ();
	public int lesskey ();
	public void add_hometable ();
	public int editchar ();
	public void init_textlist ();
	public char * forw_textlist ();
	public char * back_textlist ();
	public int edit ();
	public int edit_ifile ();
	public int edit_list ();
	public int edit_first ();
	public int edit_last ();
	public int edit_next ();
	public int edit_prev ();
	public int edit_index ();
	public IFILE save_curr_ifile ();
	public void unsave_ifile ();
	public void reedit_ifile ();
	public int edit_stdin ();
	public void cat_file ();
	public void use_logfile ();
	public char * shell_unquote ();
	public char * get_meta_escape ();
	public char * shell_quote ();
	public char * homefile ();
	public char * find_helpfile ();
	public char * fexpand ();
	public char * fcomplete ();
	public int bin_file ();
	public char * lglob ();
	public char * open_altfile ();
	public void close_altfile ();
	public int is_dir ();
	public char * bad_file ();
	public POSITION filesize ();
	public char * shell_coption ();
	public void forw ();
	public void back ();
	public void forward ();
	public void backward ();
	public int get_back_scroll ();
	public void help ();
	public void del_ifile ();
	public IFILE next_ifile ();
	public IFILE prev_ifile ();
	public IFILE getoff_ifile ();
	public int nifile ();
	public IFILE get_ifile ();
	public char * get_filename ();
	public int get_index ();
	public void store_pos ();
	public void get_pos ();
	public void set_open ();
	public int opened ();
	public void hold_ifile ();
	public int held_ifile ();
	public void * get_filestate ();
	public void set_filestate ();
	public void if_dump ();
	public POSITION forw_line ();
	public POSITION back_line ();
	public void set_attnpos ();
	public void jump_forw ();
	public void jump_back ();
	public void repaint ();
	public void jump_percent ();
	public void jump_line_loc ();
	public void jump_loc ();
	public void init_line ();
	public void prewind ();
	public void plinenum ();
	public int is_ansi_end ();
	public int pappend ();
	public void pdone ();
	public int gline ();
	public void null_line ();
	public POSITION forw_raw_line ();
	public POSITION back_raw_line ();
	public void clr_linenum ();
	public void add_lnum ();
	public LINENUM find_linenum ();
	public POSITION find_pos ();
	public LINENUM currline ();
	public void lsystem ();
	public int pipe_mark ();
	public int pipe_data ();
	public void init_mark ();
	public int badmark ();
	public void setmark ();
	public void lastmark ();
	public void gomark ();
	public POSITION markpos ();
	public void unmark ();
	public void opt_o ();
	public void opt__O ();
	public void opt_l ();
	public void opt_k ();
	public void opt_t ();
	public void opt__T ();
	public void opt_p ();
	public void opt__P ();
	public void opt_b ();
	public void opt_i ();
	public void opt__V ();
	public void opt_D ();
	public void opt_x ();
	public void opt_quote ();
	public void opt_query ();
	public int get_swindow ();
	public void scan_option ();
	public void toggle_option ();
	public int single_char_option ();
	public char * opt_prompt ();
	public int isoptpending ();
	public void nopendopt ();
	public int getnum ();
	public void init_option ();
	public struct loption * findopt ();
	public struct loption * findopt_name ();
	public int iread ();
	public void intread ();
	public long get_time ();
	public char * errno_message ();
	public int percentage ();
	public POSITION percent_pos ();
	public int  os9_signal ();
	public void put_line ();
	public void flush ();
	public int putchr ();
	public void putstr ();
	public void get_return ();
	public void error ();
	public void ierror ();
	public int query ();
	public POSITION position ();
	public void add_forw_pos ();
	public void add_back_pos ();
	public void pos_clear ();
	public void pos_init ();
	public int onscreen ();
	public int empty_screen ();
	public int empty_lines ();
	public void get_scrpos ();
	public int adjsline ();
	public void init_prompt ();
	public char * pr_expand ();
	public char * eq_message ();
	public char * pr_string ();
	public char * wait_message ();
	public void repaint_hilite ();
	public void clear_attn ();
	public void undo_search ();
	public void clr_hilite ();
	public int is_hilited ();
	public void chg_caseless ();
	public void chg_hilite ();
	public int search ();
	public void prep_hilite ();
	public RETSIGTYPE winch ();
	public RETSIGTYPE winch ();
	public void init_signals ();
	public void psignals ();
	public void cleantags ();
	public int gettagtype ();
	public void findtag ();
	public POSITION tagsearch ();
	public char * nexttag ();
	public char * prevtag ();
	public int ntags ();
	public int curr_tag ();
	public int edit_tagfile ();
	public void open_getchr ();
	public void close_getchr ();
	public int getchr ();
