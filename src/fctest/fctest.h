module fctest
  use iso_c_binding
  implicit none
  character(len=:), allocatable :: source_file
  integer :: exit_status
  character(len=132), allocatable :: msg
  interface FCE
    module procedure fctest_check_equal_int
    module procedure fctest_check_equal_real32
    module procedure fctest_check_equal_real64
    module procedure fctest_check_equal_string
  end interface FCE
  interface FCC
    module procedure fctest_check_close_real32
    module procedure fctest_check_close_real64
  end interface FCC
contains

  character(132) function sweep_leading_blanks(in_str)
    character(*), intent(in) :: in_str
    character :: ch
    integer :: j

    do j=1, len_trim(in_str)
      ! get j-th char
      ch = in_str(j:j)
      if (ch .ne. " ") then
        sweep_leading_blanks = trim(in_str(j:len_trim(in_str)))
        return
      endif
    end do
  end function sweep_leading_blanks

  subroutine ERR(line)
    integer, intent(in) :: line
    write(0,'(2A,I0,2A)') source_file,":",line,": warning: ",sweep_leading_blanks(get_source_line(line))
    exit_status=1
  end subroutine
  subroutine fctest_check_equal_int(V1,V2,line)
    integer, intent(in) :: V1, V2
    integer, intent(in) :: line
    if(V1/=V2) then
      write(0,'(2A,I0,2A)') source_file,":",line,": warning: ",sweep_leading_blanks(get_source_line(line))
      write(0,*) "--> [",V1,"!=",V2,"]"
      exit_status=1
    endif
  end subroutine
  subroutine fctest_check_equal_real32(V1,V2,line)
    real(kind=c_float), intent(in) :: V1, V2
    integer, intent(in) :: line
    if(V1/=V2) then
      write(0,'(2A,I0,2A)') source_file,":",line,": warning: ",sweep_leading_blanks(get_source_line(line))
      write(0,*) "--> [",V1,"!=",V2,"]"
      exit_status=1
    endif
  end subroutine
  subroutine fctest_check_equal_real64(V1,V2,line)
    real(kind=c_double), intent(in) :: V1, V2
    integer, intent(in) :: line
    if(V1/=V2) then
      write(0,'(2A,I0,2A)') source_file,":",line,": warning: ",sweep_leading_blanks(get_source_line(line))
      write(0,*) "--> [",V1,"!=",V2,"]"
      exit_status=1
    endif
  end subroutine           
  subroutine fctest_check_equal_string(V1,V2,line)
    character(len=*), intent(in) :: V1, V2
    integer, intent(in) :: line
    if(V1/=V2) then
      write(0,'(2A,I0,2A)') source_file,":",line,": warning: ",sweep_leading_blanks(get_source_line(line))
      write(0,*) "--> [",V1,"!=",V2,"]"
      exit_status=1
    endif
  end subroutine
  subroutine fctest_check_close_real32(V1,V2,TOL,line)
    real(kind=c_float), intent(in) :: V1, V2, TOL
    integer, intent(in) :: line
    if(.not.(abs(V1-V2)<=TOL)) then;
      write(0,'(2A,I0,2A)') source_file,":",line,": warning: ",sweep_leading_blanks(get_source_line(line))
      write(0,*) "--> [",V1,"!=",V2,"]"
      exit_status=1
    endif
  end subroutine
  subroutine fctest_check_close_real64(V1,V2,TOL,line)
    real(kind=c_double), intent(in) :: V1, V2, TOL
    integer, intent(in) :: line
    if(.not.(abs(V1-V2)<=TOL)) then;
      write(0,'(2A,I0,2A)') source_file,":",line,": warning: ",sweep_leading_blanks(get_source_line(line))
      write(0,*) "--> [",V1,"!=",V2,"]"
      exit_status=1
    endif
  end subroutine  

function get_source_line(line_number) result(source_line)
  integer, intent(in)  :: line_number
  ! Variables
  integer stat, jline
  character(132) :: source_line

  ! open input file
  open (10, file=source_file, status='old', iostat=stat)
  if (stat .ne. 0)then
    source_line = 'source_file '//source_file//' can not be opened'
    close(10)
    return
  end if

  ! process file
  do jline=1,line_number
    read (10, '(A)', end=99) source_line ! read line from input file
  enddo
  close(10)

  ! close files
  99 continue
  close (10)
end function get_source_line
                                      
end module fctest

#define TESTSUITE( TESTSUITE_NAME ) \
module TESTSUITE_NAME;\
use fctest;\
implicit none;\
contains;

#define TESTSUITE_WITH_FIXTURE( TESTSUITE_NAME, TESTSUITE_FIXTURE ) \
module TESTSUITE_NAME;\
use fctest;\
use TESTSUITE_FIXTURE;\
implicit none;\
contains;

#define END_TESTSUITE end module

#define TEST( TEST_NAME ) subroutine TEST_NAME;
#define END_TEST end subroutine;

#define TESTSUITE_INIT subroutine testsuite_init
#define END_TESTSUITE_INIT end subroutine testsuite_init

#define TESTSUITE_FINALISE subroutine testsuite_finalize
#define TESTSUITE_FINALIZE subroutine testsuite_finalize
#define END_TESTSUITE_FINALIZE end subroutine testsuite_finalize


#define CHECK( EXPR ) if(.not.(EXPR)) call ERR(__LINE__)
#define CHECK_EQUAL(V1,V2) call FCE(V1,V2,__LINE__)
#define CHECK_CLOSE(V1,V2,TOL) call FCC(V1,V2,TOL,__LINE__)