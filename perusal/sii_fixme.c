/* 	$Id$	 */
# include <dorade_headers.h>
# include <solo_window_structs.h>
# include <solo_editor_structs.h>
# include <sii_widget_content_structs.h>

/* c------------------------------------------------------------------------ */
void se_dump_bnd_files_widget (struct boundary_stuff *sebs) {}
/* c------------------------------------------------------------------------ */
void se_refresh_bnd_files_widget (struct boundary_stuff *sebs) {}
/* c------------------------------------------------------------------------ */
void se_dump_state_widget (struct solo_frame_state_files *sfsf) {}
/* c------------------------------------------------------------------------ */
void sp_refresh_delete_sweeps() {}
/* c------------------------------------------------------------------------ */
void se_dump_sed_files_widget(struct sed_command_files *scf) {}
/* c------------------------------------------------------------------------ */
void solo_refresh_list(int list_id, struct solo_list_mgmt *lm) {}
/* c------------------------------------------------------------------------ */
void se_refresh_sed_files_widget(struct sed_command_files *scf) {}
/* c------------------------------------------------------------------------ */
void se_refresh_sfic_widget(struct swp_file_input_control *sfic) {}
/* c------------------------------------------------------------------------ */
void ui_perform (char *aa) {}
/* c------------------------------------------------------------------------ */
void solo_refresh_command_buffer(struct solo_click_info *sci) {}
/* c------------------------------------------------------------------------ */
void solo_changed_parameter_info(int frame_num) {}
/* c------------------------------------------------------------------------ */
void config_from_frame_states_file() {}
/* c------------------------------------------------------------------------ */
void se_refresh_state_widget(struct solo_frame_state_files *sfsf) {}
/* c------------------------------------------------------------------------ */

void slm_list_ui(struct solo_list_mgmt *slm)
{
    char **ptr;
    int nl;

    if(!slm)
          return;

    if(!(ptr = slm->list))
          return;

    nl = slm->num_entries;

    for(; nl-- > 0;) {
       uii_printf("%s\n", *ptr++);
    }
}
/* c------------------------------------------------------------------------ */

/* c------------------------------------------------------------------------ */

/* c------------------------------------------------------------------------ */


