/*	$OpenBSD: src/usr.bin/vim/proto/Attic/screen.pro,v 1.2 1996/09/21 06:23:54 downsj Exp $	*/
/* screen.c */
void updateline __PARMS((void));
void update_curbuf __PARMS((int type));
void updateScreen __PARMS((int type));
void updateWindow __PARMS((WIN *wp));
void win_update __PARMS((WIN *wp));
void status_redraw_all __PARMS((void));
void win_redr_status __PARMS((WIN *wp));
void display_dollar __PARMS((colnr_t col));
void undisplay_dollar __PARMS((void));
void screen_outchar __PARMS((int c, int row, int col));
void screen_msg __PARMS((char_u *text, int row, int col));
void screen_start __PARMS((void));
int set_highlight __PARMS((int context));
void start_highlight __PARMS((void));
void stop_highlight __PARMS((void));
void remember_highlight __PARMS((void));
void recover_old_highlight __PARMS((void));
void screen_fill __PARMS((int start_row, int end_row, int start_col, int end_col, int c1, int c2));
void comp_Botline_all __PARMS((void));
void comp_Botline __PARMS((WIN *wp));
void screenalloc __PARMS((int clear));
void screenclear __PARMS((void));
void check_cursor __PARMS((void));
void cursupdate __PARMS((void));
void scroll_cursor_top __PARMS((int min_scroll, int always));
void scroll_cursor_bot __PARMS((int min_scroll, int set_topline));
void scroll_cursor_halfway __PARMS((int atend));
void cursor_correct __PARMS((void));
int curs_rows __PARMS((void));
void curs_columns __PARMS((int scroll));
void scrolldown __PARMS((long line_count));
void scrollup __PARMS((long line_count));
void scrolldown_clamp __PARMS((void));
void scrollup_clamp __PARMS((void));
int win_ins_lines __PARMS((WIN *wp, int row, int line_count, int invalid, int mayclear));
int win_del_lines __PARMS((WIN *wp, int row, int line_count, int invalid, int mayclear));
void win_rest_invalid __PARMS((WIN *wp));
int screen_del_lines __PARMS((int off, int row, int line_count, int end, int force));
int showmode __PARMS((void));
void delmode __PARMS((void));
void showruler __PARMS((int always));
void win_redr_ruler __PARMS((WIN *wp, int always));
int screen_valid __PARMS((int clear));
int jump_to_mouse __PARMS((int flags));
void redraw_later __PARMS((int type));
