



/****************************************************************************/
void sp_update_frame_info(int frame)
{
    WW_PTR wwptr, solo_return_wwptr();

    wwptr = solo_return_wwptr(frame);
    if((windptr->frame_config < 4) || (windptr->frame_config == 6)){
       wwptr->view->width_in_pix = windptr->dwidth;
       wwptr->view->height_in_pix = windptr->dheight;
    }
    else{
       wwptr->view->width_in_pix = 
                       widgetptr->LargeImages[frame]->width;
       wwptr->view->height_in_pix = 
                       widgetptr->LargeImages[frame]->height;
    }


    if((windptr->frame_config < 4) || (windptr->frame_config == 6)){
       wwptr->image = widgetptr->Images[frame];
    }
    else{
       wwptr->image = widgetptr->LargeImages[frame];
    }
}/*sp_update_frame_info*/
